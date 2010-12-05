
var w = 640, h = 480, title = "Fractal", white, running = true;
var halfW = w / 2, halfH = h / 2;

var win = new SDL.Window(title, w, h);

white = win.color("#ffffff");

var x1 = halfW, x2 = halfW;
var y1 = 0, y2 = 0;

for(var iterate = 0; iterate < 10000000; iterate++)
{
    var Direct = Math.floor(Math.random() * 3);
    
    if(Direct == 0)
    {
        x1 = (x2 + halfW) / 2;
        y1 = (y2 + 0) / 2;  
    }
    else if(Direct == 1)
    {
        x1 = (x2 + 0) / 2;
        y1 = (y2 + h) / 2;
    }
    else if(Direct == 2)
    {
        x1 = (x2 + w) / 2;    
        y1 = (y2 + h) / 2;
    }
    win.pixel(x1, y1, white);
    if(!(iterate%1000))
        win.update();
    
    x2 = x1;
    y2 = y1;
}
win.update();

while(running) win.awaitEvent({
    quit: function() {
        running = false;
    }
});