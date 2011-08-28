var elm = require("elm");

var win = new elm.Win(null, "Hello World");
win.resize(320,480);
var bg = new elm.Bg(win, "/home/sanjeev/Pictures/computer.png");
print("Label test = " + win.title"]);
win.title"] = "Changing world";
var box = new elm.Box(win);

var bt1 = new elm.Button(win);
bt1.label = "Setting 1";
box.add(bt1);

var bt2 = new elm.Button(win);
bt2.label = "Setting 2";
box.add(bt2);

var bt3 = new elm.Button(win);
bt3.label = "Setting 3";
box.add(bt3);

var bt4 = new elm.Button(win);
bt4.label = "Setting 4";
box.add(bt4);

var bt5	= new elm.Button(win);
bt5.label = "Setting 5";
box.add(bt5);
