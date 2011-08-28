var SDL = require("SDL");
var w = 1200, title = "Mandelbrot", colors = [], running = true;

var square = {min:{x:-2,y:-1}, max:{x:1,y:1}}, zoom = 1, pan = {x:0,y:0};//zoom = 3/.00073801, pan = {x:-.74364990, y:-.13188204};
square.min.x /= zoom;
square.min.y /= zoom;
square.max.x /= zoom;
square.max.y /= zoom;
square.min.x += pan.x;
square.max.x += pan.x;
square.min.y += pan.y;
square.max.y += pan.y;
square.w = square.max.x - square.min.x;
square.h = square.max.y - square.min.y;
var h = w/square.w*square.h;
square.x = -w/square.w*square.min.x;
square.y = -h/square.h*square.min.y;
square.scaleW = w/square.w;
square.scaleH = h/square.h;

var zero = {x:0, y:0};

function sum(a, b) {
    return {x:(a.x+b.x), y:(a.y+b.y)};
}

function mul(a, b) {
    return {x:(a.x*b.x-a.y*b.y), y:(a.y*b.x+a.x*b.y)};
}

function pow2(a) {
    return {x:(a.x*a.x-a.y*a.y), y:(2*a.x*a.y)};
}

function mod(a) {
    return Math.sqrt(a.x*a.x+a.y*a.y);
}

function M(z, c) {
    return sum(pow2(z), c);
}

var r = Math.random()*9;

var maxMu = 0;

function log10(x) {
    return Math.log(x)/Math.LN10;
}

function log2(x) {
    return Math.log(x)/Math.LN2;
}

function ln(x) {
    return Math.log(x);
}

function isInM(x, y) {
    var z = zero, c = {x:x, y:y}, mz;
    for(var i = 0; i < 1000; i++) {
        z = M(z, c);
        mz = mod(z);
        if(mz > 2) {
            var mu = (i + log2(128) - log2(ln(mz))) * 0.05;
            //if(mu > maxMu)
            //   print((maxMu=mu)+" "+i+" "+mz);
            //return Math.floor(mu*1024);
            return 400-Math.floor(mu*128*0.48)%400;
        }
            //return Math.floor(((i+r)*r)%1023+1);
    }
    return 0;
}

function dis(begin, end, x) {
    return Math.floor(begin+(end-begin)*x);
}

function breakColor(a, t) {
    return {r:Math.floor((a>>16)%256), g:Math.floor((a>>8)%256), b:Math.floor(a%256), t:t};
}

var colorSpaces = [breakColor(0, 0), breakColor(6555392, 28), breakColor(13331232, 92), breakColor(16777197, 196), breakColor(43775, 285), breakColor(3146289, 371), breakColor(0, 400)];
print(JSON.stringify(colorSpaces));
//index=28 color=6555392 index=92 color=13331232
//index=196 color=16777197 index=285 color=43775 index=371 color=3146289

var win = new SDL.Window(title, w, h);

var colorSpace = 0, i = 0, lastIndex = 0;
while(colorSpace < colorSpaces.length-1) {
    var a = colorSpaces[colorSpace], b = colorSpaces[colorSpace+1];
    var colorInSpace = (i-a.t)/(b.t-a.t);
    colors[i++] = win.color(dis(a.r, b.r, colorInSpace), dis(a.g, b.g, colorInSpace), dis(a.b, b.b, colorInSpace));
    if(i == b.t+1)
        colorSpace++;
}
//colors[1024] = win.color(0, 0, 0);
/*

for(var i = 0; i < 256; i++)
    colors[i] = win.color(dis(0, 127, i), dis(0, 127, i), dis(0, 255, i));
for(var i = 0; i < 256; i++)
    colors[256+i] = win.color(dis(127, 255, i), dis(127, 0, i), dis(255, 127, i));
for(var i = 0; i < 256; i++)
    colors[512+i] = win.color(dis(255, 0, i), dis(0, 255, i), dis(127, 255, i));
for(var i = 0; i < 256; i++)
    colors[256+512+i] = win.color(dis(0, 255, i), dis(255, 127, i), dis(255, 127, i));
colors[1024] = win.color(255, 127, 127);*/
square.xScaled = square.x/square.scaleW;
square.yScaled = square.y/square.scaleH;
square.halfPixelW = .5/square.scaleW;
square.halfPixelH = .5/square.scaleH;
for(var y = 0; y < h; y++) {
    for(var x = 0; x < w; x++) {
        //win.pixel(x, y, colors[Math.floor(x/w*colors.length)]);continue;
        x=x;y=y;
        var cx = x/square.scaleW - square.xScaled, cy = y/square.scaleH - square.yScaled;
        //var a = isInM(cx-square.halfPixelW, cy-square.halfPixelH), b = isInM(cx+square.halfPixelW, cy-square.halfPixelH),
        //    c = isInM(cx-square.halfPixelW, cy+square.halfPixelH), d = isInM(cx+square.halfPixelW, cy+square.halfPixelH);
        //win.pixel(x, y, colors[Math.floor((a+b+c+d)/4)]);
        var m = isInM(cx, cy);
        win.pixel(x, y, colors[m]);
    }
    if(!(y%5))
        win.update();
}
win.update();

while(running) win.awaitEvent({
    quit: function() {
        running = false;
    }
});
