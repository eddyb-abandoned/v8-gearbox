var w = 640, h = 480, title = "<3", colors = [], running = true;
var halfW = w/2, halfH = h/2;

var triangle1H = halfH;
var triangle1W = triangle1H * Math.sqrt(5 / 3);
var triangle2W = triangle1W;
var triangle2H = triangle2W / 2 * Math.tan(Math.PI/6);

var triangle2Y = h - triangle1H - triangle2H + 1;
var offsetY = (triangle2Y - triangle2Y+triangle2H*3/2 ) / 2 - 16;
triangle2Y -= offsetY;

var win = new SDL.Window(title, w, h);

//for(var i = 0; i < 1024; i++)
//    colors[i] = win.color(i / 4, i % 256, (i % 512)/2);
var pink = win.color(255, 64, 255);
var pinkLight = win.color(255, 128, 255);

SDL.Window.prototype.fillCircle = function(ox, oy, radius, color) {
    for(var y=-radius; y<=radius; y++)
        for(var x=-radius; x<=radius; x++)
            if(x*x+y*y <= radius*radius)
                this.pixel(ox+x, oy+y, color);
    return this;
}

for(var y = 0; y < h; y++)
    for(var x = 0; x < w; x++)
        win.pixel(x, y, pinkLight);

for(var y = 0; y < triangle1H; y++)
    for(var x = 0; x < y/triangle1H*triangle1W/2; x++)
        win.pixel(halfW+x, h-y-offsetY, pink).pixel(halfW-x, h-y-offsetY, pink);
for(var y = 0; y < triangle2H; y++)
    for(var x = 0; x < y/triangle2H*triangle2W/2; x++)
        win.pixel(halfW+x, triangle2Y+y, pink).pixel(halfW-x, triangle2Y+y, pink);

win.fillCircle(halfW-triangle2W/4, triangle2Y+triangle2H/2, triangle2H, pink).fillCircle(halfW+triangle2W/4, triangle2Y+triangle2H/2, triangle2H, pink);
win.update();

while(running) win.awaitEvent({
    quit: function() {
        running = false;
    }
});