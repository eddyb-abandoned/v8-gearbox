
var w = 640, h = 480, title = "Test", colors = [], running = true;

var win = new SDL.Window(title, w, h);

for(var i = 0; i < 1024; i++)
    colors[i] = win.color(i / 4, i % 256, (i % 512)/2);

var raport = 1024 / Math.sqrt(w*w/4 + h*h);

for(var x = 0; x < w / 2; x++)
    for(var y = 0; y < h; y++)
        win.pixel(x, y, colors[Math.floor(Math.sqrt(x*x + y*y) * raport)]).pixel(w - x - 1, y, colors[Math.floor(Math.sqrt(x*x + y*y) * raport)]);
win.update();

while(running) win.awaitEvent({
    quit: function() {
        running = false;
    }
});