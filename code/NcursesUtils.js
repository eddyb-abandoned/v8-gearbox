Ncurses.A_ALTCHARSET = 1 << 22;
function NCURSES_ACS(c){return c.charCodeAt() | Ncurses.A_ALTCHARSET;}
Ncurses.ACS_ULCORNER = NCURSES_ACS('l') /* upper left corner */
Ncurses.ACS_LLCORNER = NCURSES_ACS('m') /* lower left corner */
Ncurses.ACS_URCORNER = NCURSES_ACS('k') /* upper right corner */
Ncurses.ACS_LRCORNER = NCURSES_ACS('j') /* lower right corner */
Ncurses.ACS_LTEE = NCURSES_ACS('t') /* tee pointing right */
Ncurses.ACS_RTEE = NCURSES_ACS('u') /* tee pointing left */
Ncurses.ACS_BTEE = NCURSES_ACS('v') /* tee pointing up */
Ncurses.ACS_TTEE = NCURSES_ACS('w') /* tee pointing down */

Ncurses.wrapWindow = function(x, y, cols, rows, title) {
    var borderWindow = new Ncurses.Window(x, y, cols, rows);
    borderWindow.border();
    if(title) {
        borderWindow.move(cols/2-title.length/2-1, 0);
        borderWindow.print(" "+title+" ");
    }
    return new Ncurses.Window(x+1, y+1, cols-2, rows-2);
}

Ncurses.tabbedWindow = function(x, y, cols, rows) {
    this.x = x;
    this.y = y;
    this.cols = cols;
    this.rows = rows;
    var borderWindow = new Ncurses.Window(this.x, this.y + 2, this.cols, this.rows - 2);
    borderWindow.border();
}

Ncurses.tabbedWindow.prototype.contentWindows = {};
Ncurses.tabbedWindow.prototype.tabWindows = {};

Ncurses.tabbedWindow.prototype.x = 0;
Ncurses.tabbedWindow.prototype.y = 0;
Ncurses.tabbedWindow.prototype.cols = 0;
Ncurses.tabbedWindow.prototype.rows = 0;

Ncurses.tabbedWindow.prototype.activeTab = undefined;
Ncurses.tabbedWindow.prototype.tabs = [];
Ncurses.tabbedWindow.prototype.newTabX = 2;

Ncurses.tabbedWindow.prototype.addTab = function(tabName) {
    if(this.tabs.indexOf(tabName) != -1)
        return;
    this.contentWindows[tabName] = new Ncurses.Window(this.x + 1, this.y + 3, this.cols - 2, this.rows - 4);
    this.contentWindows[tabName].clear();
    this.tabWindows[tabName] = new Ncurses.Window(this.x + this.newTabX, this.y, tabName.length + 2, 3);
    this.tabWindows[tabName].move(1, 1);
    this.tabWindows[tabName].print(tabName);
    this.newTabX += tabName.length + 3;
    this.tabs.push(tabName);
    this.switchTab(tabName);
}

Ncurses.tabbedWindow.prototype.switchTab = function(tabName) {
    if(!tabName)
        return;
    if(this.activeTab) {
        this.contentWindows[this.activeTab].autoRefresh = false;
        this.tabWindows[this.activeTab].border(0, 0, Ncurses.ACS_BTEE, Ncurses.ACS_BTEE);
    }
    
    this.contentWindows[tabName].autoRefresh = true;
    this.contentWindows[tabName].touch();
    this.contentWindows[tabName].print("");
    this.tabWindows[tabName].move(1, 1);
    this.tabWindows[tabName].print(tabName);
    this.tabWindows[tabName].border(0, 0, Ncurses.ACS_LRCORNER, Ncurses.ACS_LLCORNER);
    this.tabWindows[tabName].move(1, 2);
    for(var i = 0; i < tabName.length; i++)
        this.tabWindows[tabName].print(" ");
    this.activeTab = tabName;
}

Ncurses.tabbedWindow.prototype.goForward = function() {
    var idx = this.tabs.indexOf(this.activeTab);
    if(idx == -1 || idx == (this.tabs.length - 1))
        return;
    this.switchTab(this.tabs[++idx]);
}

Ncurses.tabbedWindow.prototype.goBackward = function() {
    var idx = this.tabs.indexOf(this.activeTab);
    if(idx == -1 || !idx)
        return;
    this.switchTab(this.tabs[--idx]);
}

Ncurses.tabbedWindow.prototype.flash = function(tabName) {
    this.tabWindows[tabName].move(1, 1);
    this.tabWindows[tabName].bold(true);
    this.tabWindows[tabName].print(tabName);
    this.tabWindows[tabName].bold(false);
}