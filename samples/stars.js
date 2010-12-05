
var w = 640, h = 480, title = "Stars", colors = [], stars = [], starsr = [], old = [], angle = 0, running = true;
var halfW = w / 2, halfH = h / 2;

var win = new SDL.Window(title, w, h);

for(var i = 0; i < 256; i++)
    colors[i] = win.color(i, i, i);

for(var i = 0; i < 100; i++) {
    stars[i] = [];
    starsr[i] = [];
    old[i] = [];
    stars[i][0] = ((Math.random() * 320) + 1 - 160) * 3;
    stars[i][1] = ((Math.random() * 320) + 1 - 160) * 3;
    stars[i][2] = ((Math.random() * 128) + 1 - 64) * 5;
}

function sin(x) {
    return Math.sin(x*Math.PI/180);
}

function cos(x) {
    return Math.cos(x*Math.PI/180);
}

function Transform(n, a, b)
{
    var y = 0.0, z = 0.0;
    y = (cos((angle / 20)) * starsr[n][a]) - (sin((angle / 20)) * starsr[n][b]);
    z = (sin((angle / 20)) * starsr[n][a]) + (cos((angle / 20)) * starsr[n][b]);
    starsr[n][a] = Math.floor(y);
    starsr[n][b] = Math.floor(z);
}
var started = (new Date()).getTime(), frames = 0;
while(running) {
    
    for (i = 0; i < 100; i++)
    {
        old[i][0] = starsr[i][0];
        old[i][1] = starsr[i][1];
        old[i][2] = starsr[i][2];
        
        starsr[i][0] = stars[i][0];
        starsr[i][1] = stars[i][1];
        starsr[i][2] = stars[i][2];
        
        Transform(i, 1, 2);
        Transform(i, 0, 2);
        Transform(i, 0, 1);
        
        var oldx = Math.floor(((256*old[i][0])/(old[i][2]-1024))+halfW);
        var oldy = Math.floor(((256*old[i][1])/(old[i][2]-1024))+halfH);
        
        var x = Math.floor(((256*starsr[i][0])/(starsr[i][2]-1024))+halfW);
        var y = Math.floor(((256*starsr[i][1])/(starsr[i][2]-1024))+halfH);
        
        color = Math.floor((starsr[i][2] + 721) / 5.5);
        win.pixel(oldx, oldy, colors[0]).pixel(x, y, colors[color]);
    }
    win.update();
    angle += 0.5;
    frames++;
    win.checkEvent({
        quit: function() {
            var secondsPassed = ((new Date()).getTime() - started) / 1000;
            print("FPS: " + (frames / secondsPassed));
            running = false;
        }
    });
}