var elm = require("Elm");

var win = new elm.Win(null, "Hello World");
win.resize(320,480);
var bg = new elm.Background(win, "./samples/images/v8.png", null);
print("Label test = " + win.title);
win.title = "Changing world";
var box = new elm.Box(win);
box.horizontal = true;

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
print("Setting CB in JS");
bt5.onClick = function() {
                    print("Clicked Button #5 ");
                 };
box.add(bt5);

elm.mainLoop();
