var elm = require("elm");
var win = new elm.Win(null, "Hello World");
win.resize(320,480);
var bg = new elm.Bg(win, null, null);
bg.red=200;
bg.green=125;
win.title = "Changing world";
var box = new elm.Box(win);
box.horizontal = false;

var icon = new elm.Icon(win,"samples/images/v8.png");
icon.xalign = 0.5;
icon.yalign = 0.0;
box.add(icon);

elm.mainLoop();
