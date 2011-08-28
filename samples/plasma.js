var SDL = require("SDL");
var w = 320, h = 200, title = "Plasma", colors = [], r = [], R = [], t = [], running = true;
var tableW = w * 2, tableH = h * 2;

var win = new SDL.Window(title, w, h);

function int(x, sz) {
    if(sz)
        return Math.floor(x % (1 << sz));
    else
        return Math.floor(x);
}

function char(x) {
    return int(x, 8);
}

for(var c = 0; c < 3; c++)
    r[c] = (Math.random()*1000+1)/300000;
for(var c = 0; c < 6; c++)
    R[c] = (Math.random()*1000+1)/5000;

t.p = function(x, y) {
    return this[x + y * tableW];
};

for(var y = 0; y < tableH; y++) {
    for(var x = 0; x < tableW; x++) {
        var tmp=(((x-(tableW/2))*(x-(tableW/2))+(y-(tableW/2))*(y-(tableW/2)))*(Math.PI/(tableW*tableW+tableH*tableH)));
        t[y*tableW+x]=char((Math.sin(Math.sqrt(tmp)*12)+1)*256/6);
    }
}


function do_plasma(x1, y1, x2, y2, x3, y3) {
    for(var y=0; y < h; y++) 
        for(var x = 0; x < w; x++)
            win.pixel(x, y, colors[char(t.p(x+x1, y+y1)+t.p(x+x2, y+y2)+t.p(x+x3, y+y3))]);
}

var started = (new Date()).getTime(), frames = 0;
while(running) {
    frames++;
    
    for(var i = 0; i < 256; i++) {
        var _r=(Math.sin(i/256*6*Math.PI+r[0]*Math.PI*frames*Math.PI)+1)*127;
        var _g=(Math.sin(i/256*6*Math.PI+r[1]*frames*Math.PI)+1)*127;
        var _b=(Math.sin(i/256*6*Math.PI+r[2]*frames*Math.PI)+1)*127;
        colors[i] = win.color(_r, _g, _b);
    }
    
    do_plasma(int((Math.sin(frames*R[0])+1)/2*w),
              int((Math.sin(frames*R[1])+1)/2*h),
              int((Math.sin(frames*R[2])+1)/2*w),
              int((Math.sin(frames*R[3])+1)/2*h),
              int((Math.sin(frames*R[4])+1)/2*w),
              int((Math.sin(frames*R[5])+1)/2*h));
    
    win.update();
    win.checkEvent({
        quit: function() {
            var secondsPassed = ((new Date()).getTime() - started) / 1000;
            print("FPS: " + int(frames / secondsPassed));
            running = false;
        }
    });
}
