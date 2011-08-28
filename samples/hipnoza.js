var SDL = require("SDL");
var w = 320, h = 240, title = "Hipnoza", colors = [], diagTable = [], offset = 0, running = true, mijto = true;
var halfW = w / 2, halfH = h / 2;

var win = new SDL.Window(title, w * 2, h);

for(var i = 0; i <= 128; i++)
    colors[i] = win.color(Math.floor(255 - Math.abs(128 - i * 2) * 2), Math.floor(127 - Math.abs(128 - i * 2)), Math.floor(255 - Math.abs(128 - i * 2) * 2));

function diagonal(x, y) {
    return Math.sqrt(x*x + y*y);
}

var maxDiag = diagonal(halfW, halfH);

var started = (new Date()).getTime(), frames = 0;
while(running) {
    for(var i = 0; i < maxDiag; i++)
        diagTable[i] = colors[Math.abs(Math.floor((i * 512 / maxDiag + offset) % 128))];
    for(var x = 0; x <= halfW; x++) {
        for(var y = 0; y <= halfH; y++) {
            var color = diagTable[Math.floor(diagonal(x, y))];
            win.pixel(halfW-x, halfH-y, color).pixel(halfW+x, halfH-y, color).pixel(halfW-x, halfH+y, color).pixel(halfW+x, halfH+y, color);
            win.pixel(halfW-x+w, halfH-y, color).pixel(halfW+x+w, halfH-y, color).pixel(halfW-x+w, halfH+y, color).pixel(halfW+x+w, halfH+y, color);
        }
        win.checkEvent({
            quit: function() {
                var secondsPassed = ((new Date()).getTime() - started) / 1000;
                print("FPS: " + (frames / secondsPassed));
                running = false;
            }
        });
    }
    win.update();
    if(mijto)
        offset += Math.random()*100-50;
    else
        offset += 30;
    frames++;
}
