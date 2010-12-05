load("code/NcursesUtils.js");

Array.fromObject = function(obj) {
    var arr = [];
    for(var i in obj)
        arr.push(obj[i]);
    return arr;
}

function IrcClient(server, nick, channels) {
    this.server = server;
    this.nick = nick;
    this.joinChannels = channels;
}

IrcClient.commandList = [
{sender:true, cmd:"NOTICE", func:function(args){return [args.client.mainWindow.activeTab, "[Notice] -"+args[0]+"- "+args[2]];}},
{sender:true, cmd:"PRIVMSG", func:function(args){
    var nick = args[0].split('!')[0], message = "<"+nick+"> "+args[2];
    if(args[2].charCodeAt() == 1) {
        var special = args[2].slice(1, -1).split(' '), action = special[0], rest = special.slice(1).join(' ');
        if(action == "VERSION")
            return [args.client.mainWindow.activeTab, "[CTCP] Received Version request from "+nick+"."];
        if(action == "ACTION")
            message = "* "+nick+" "+rest;
        else
            return [args.client.mainWindow.activeTab, "[CTCP] Received Unknown "+action+" request from "+nick+"."];
    }
    if(args[1] == args.client.nick) {
        args.client.mainWindow.addTab(nick);
        return [nick, message];
    }
    return [args[1], message];
}},
{sender:true, cmd:"JOIN", func:function(args){
    var nick=args[0].split('!')[0];
    if(nick == args.client.nick) {
        args.client.mainWindow.addTab(args[1]);
        return [args[1], "--> You have joined the channel "+args[1]+" ("+args[0]+")."];
    }
    return [args[1], "--> "+nick+" has joined this channel ("+args[0]+")."];
}},
{cmd:"PING", func:function(args){args.client.sendCommand("PONG", "none");}},
{sender:true, cmd:'001', func:function(args){return [args.client.server, "[Welcome] "+args[2]];}},
{sender:true, cmd:'002', func:function(args){return [args.client.server, "[Welcome] "+args[2]];}},
{sender:true, cmd:'003', func:function(args){return [args.client.server, "[Welcome] "+args[2]];}},
{sender:true, cmd:'004', func:function(args){return [args.client.server, "[Welcome] "+args[2]];}},
{sender:true, cmd:'372', func:function(args){return [args.client.server, "[MOTD] "+args[2]];}},
];


IrcClient.prototype.server = "";
IrcClient.prototype.nick = "";
IrcClient.prototype.channels = [];

IrcClient.prototype.conn = undefined;

IrcClient.prototype.mainWindow = undefined;
IrcClient.prototype.inputWindow = undefined;

IrcClient.prototype.running = false;
IrcClient.prototype.ncs = false;

IrcClient.prototype.init = function() {
    Ncurses.enter();
    this.ncs = true;
    var cols = Ncurses.cols(), rows = Ncurses.rows();
    this.mainWindow = new Ncurses.tabbedWindow(0, 0, cols, rows - 1);
    this.mainWindow.addTab(this.server);
    this.inputWindow = new Ncurses.Window(0, rows - 1, cols, 1);
    this.inputWindow.clear();
    this.inputWindow.print("> ");
    
    this.conn = new Network.TcpConnection(this.server, 6667);
    this.sendCommand("NICK", this.nick);
    this.sendCommand("USER", this.nick, "none", "none", ":JavaScript IRC Client");
    for(c in this.joinChannels)
        this.sendCommand("JOIN", this.joinChannels[c]);
    this.running = true;
}

IrcClient.prototype.deinit = function() {
    if(this.ncs) {
        this.ncs = false;
        Ncurses.exit();
    }
}

IrcClient.prototype.sendCommand = function() {
    if(arguments.length>=1)
        return this.conn.send(Array.fromObject(arguments).join(" ")+"\r\n");
}

IrcClient.prototype.process = function(input) {
    for(i in input) {
        var line = input[i].trimRight(), res = false;
        for(j in IrcClient.commandList) {
            var cmd = IrcClient.commandList[j];
            var mt = line.match(new RegExp('^'+(cmd.sender?'\:([^ ]+) ':'')+cmd.cmd+'((?: [^\: ]*)*)(?: \:(.*))?$')), args = [], idx = 1;
            if(mt) {
                if(cmd.sender)
                    args.push(mt[idx++]);
                if(mt[idx])
                    args = args.concat(mt[idx].trimLeft().split(" "));
                if(mt[idx+1])
                    args.push(mt[idx+1]);
                args.client = this;
                res = cmd.func(args);
                break;
            }
        }
        if(res && res[0] && res[1] && this.mainWindow.contentWindows[res[0]]) {
            var now = new Date();
            this.mainWindow.contentWindows[res[0]].print("["+now.getHours()+":"+now.getMinutes()+"] "+res[1]+"\n");
            if(res[0] != this.mainWindow.activeTab)
                this.mainWindow.flash(res[0]);
        }
    }
}

IrcClient.prototype.main = function() {
    this.init();
    var recv, input = "", char;
    while(this.running) {
        var recv = this.conn.receive();
        if(recv) {
            var toParse = recv.split("\r\n");
            this.process(toParse);
        }
        while(char = this.inputWindow.getChar()) {
            char = String.fromCharCode(char);
            if(char =='\n') {
                if(!input)
                    continue;
                var cmd = "PRIVMSG "+this.mainWindow.activeTab+" :"+input, selfPrefix = ":"+this.nick+" ";
                if(input.charAt() == '/') {
                    input = input.slice(1).split(" ");
                    switch(input[0].toLowerCase()) {
                        case 'join':
                            if(input[1].charAt() != '#')
                                input[1] = '#' + input[1];
                            cmd = "JOIN " + input[1];
                            selfPrefix = undefined;
                            break;
                        case 'query':
                            this.mainWindow.addTab(input[1]);
                            cmd = undefined;
                            break;
                        case 'nick':
                            cmd = "NICK " + input[1];
                            this.nick = input[1];
                            break;
                        case 'eval':
                            result = eval(input.slice(1).join(" "));
                            cmd = "PRIVMSG "+this.mainWindow.activeTab+" :"+result;
                            break;
                        case 'me':
                            cmd = "PRIVMSG "+this.mainWindow.activeTab+" :\x01ACTION "+input.slice(1).join(" ")+"\x01";
                            break;
                    }
                }
                if(cmd) {
                    this.sendCommand(cmd);
                    if(selfPrefix)
                        this.process([selfPrefix + cmd]);
                }
                input = "";
                this.inputWindow.clear();
                this.inputWindow.move(0, 0);
                this.inputWindow.print("> ");
                continue;
            }
            if(char.charCodeAt() == 127) {
                input = input.slice(0, -1);
                this.inputWindow.print("\b \b");
                continue;
            }
            if(input.charCodeAt(0) == 27 && input.charCodeAt(1) == 91) {
                if(char.charCodeAt() == 68)
                    this.mainWindow.goBackward();
                else if(char.charCodeAt() == 67)
                    this.mainWindow.goForward();
                else
                    this.inputWindow.print(" CONTROL["+char.charCodeAt()+"]");
                input = "";
                this.inputWindow.clear();
                this.inputWindow.move(0, 0);
                this.inputWindow.print("> ");
                continue;
            }
            input += char;
            this.inputWindow.print(char);
        }
    }
    this.deinit();
}