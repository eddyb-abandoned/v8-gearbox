var elm = require("elm");
var win = new elm.Win(null, "Hello World");
win.resize(320,480);
var image = new elm.Image(win, "/home/sanjeev/Pictures/computer.png");
elm.mainLoop();
