var SDL = require("SDL");
var w = 1200, title = "Fractal", colors = [], black, running = true;

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

function div(a, b) {
    return {x:(a.x*b.x+a.y*b.y)/(b.x*b.x+b.y*b.y), y:(a.y*b.x-a.x*b.y)/(b.x*b.x+b.y*b.y)};
}

function pow2(a) {
    return {x:(a.x*a.x-a.y*a.y), y:(2*a.x*a.y)};
}

function mod(a) {
    return Math.sqrt(a.x*a.x+a.y*a.y);
}

function M(z, c) {
    return sum(mul(z, {x:Math.sin(c.x), y:Math.cos(c.y)}), c);
}

function log10(x) {
    return Math.log(x)/Math.LN10;
}

function log2(x) {
    return Math.log(x)/Math.LN2;
}

function ln(x) {
    return Math.log(x);
}

function polarAngle(x, y) {
    y = -y;
    if(!x && !y) return 0;
    if(!x && y>0) return Math.PI/2;
    if(!x && y<0) return 3*Math.PI/2;
    var at = Math.atan(y/x);
    if(x<0) return at+Math.PI;
    if(x>0 && y<0) return at+2*Math.PI;
    if(x>0 && y>=0) return at;
}

var deg60 = Math.PI/3, deg30 = Math.PI/6;

// Fractal stuff
var fractals = {};
fractals.mandelbrot = {
    maxIter:1000,
    init:function(ctx) {
        ctx.z = zero;
    },
    loop:function(ctx) {
        ctx.z = sum(pow2(ctx.z), ctx.pixel);
    },
    inside:function(ctx) {
        return mod(ctx.z) <= 2;
    },
    colorInside:function(ctx) {
        return black;
    },
    colorOutside:function(ctx) {
        var mu = (ctx.iter + log2(128) - log2(ln(mod(ctx.z)))) * 0.05;
        return colors[399-Math.floor(mu*128*0.48)%400];
    }
};

function inWhichFlower(a) {
    var wx = Math.floor(a.x/.8+.5), wy = Math.floor(a.y/.8+.5);
    return {x:wx*.8, y:wy*.8};
}

fractals.flowers = {
    maxIter:1,
    init:function(ctx) {
    },
    loop:function(ctx) {
    },
    inside:function(ctx) {
        var flower = inWhichFlower(ctx.pixel);
        var r = mod({x:ctx.pixel.x-flower.x, y:ctx.pixel.y-flower.y}), angle = polarAngle(ctx.pixel.x-flower.x, ctx.pixel.y-flower.y);
        //if(r<.1)
        //print(angle);//(.5-Math.abs((angle%deg60)/deg60-.5))*.1);
        ctx.rose = .15*Math.sin(angle*7)+.25;
        ctx.r = r;
        return r < ctx.rose && Math.floor(200+ctx.r/ctx.rose*400)%400 > 200;
    },
    colorInside:function(ctx) {
        //return colors[200];
        return colors[Math.floor(200+ctx.r/ctx.rose*400)%400];
    },
    colorOutside:function(ctx) {
        return colors[Math.floor(200+mod(ctx.pixel)/2*100)];
    }
};

fractals.casa = {
    maxIter:1,
    init:function(ctx) {
    },
    loop:function(ctx) {
    },
    inside:function(ctx) {
        if(ctx.pixel.y < -2)
            return Math.abs(ctx.pixel.x) <= 4+ctx.pixel.y;
        return (Math.abs(ctx.pixel.x) <= 1.5) && (ctx.pixel.y >= -2) && (ctx.pixel.y <= 1);
    },
    colorInside:function(ctx) {
        //return colors[0];
        return colors[Math.floor(20+(mod(ctx.pixel)+10)*25)%400];
    },
    colorOutside:function(ctx) {
        return colors[Math.floor(200+mod(ctx.pixel)*10)];
    }
};

fractals.test = {
    maxIter:1,
    init:function(ctx) {
    },
    loop:function(ctx) {
    },
    inside:function(ctx) {
        var r = mod({x:ctx.pixel.x, y:ctx.pixel.y}), angle = polarAngle(ctx.pixel.x, ctx.pixel.y);
        ctx.a = angle;
        return r < .2*Math.sin(50*angle)+.5;//+Math.sqrt(2*r/5);
    },
    colorInside:function(ctx) {
        //return colors[200];
        //return colors[Math.floor(50+(ctx.a%(Math.PI/5*2)+.5)*40)%400];
        return colors[Math.floor(80+mod(ctx.pixel)*80)];
    },
    colorOutside:function(ctx) {
        return colors[Math.floor(200+mod(ctx.pixel)*50)];
    }
};

var fractal = fractals.test;

function doFractal(x, y) {
    var ctx = {pixel:{x:x,y:y}, iter:0};
    fractal.init(ctx);
    for(;ctx.iter < fractal.maxIter; ctx.iter++) {
        fractal.loop(ctx);
        if(!fractal.inside(ctx))
            return fractal.colorOutside(ctx);
    }
    return fractal.colorInside(ctx);
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
black = win.color(0, 0, 0);
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
        //var a = doFractal(cx-square.halfPixelW, cy-square.halfPixelH), b = doFractal(cx+square.halfPixelW, cy-square.halfPixelH),
        //    c = doFractal(cx-square.halfPixelW, cy+square.halfPixelH), d = doFractal(cx+square.halfPixelW, cy+square.halfPixelH);
        //win.pixel(x, y, colors[Math.floor((a+b+c+d)/4)]);
        var m = doFractal(cx, cy);
        win.pixel(x, y, m);
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
