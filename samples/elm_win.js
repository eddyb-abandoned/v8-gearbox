var elm = require("Elm");
var win = new elm.Win(null, "Hello World");
win.resize(320,480);
var bg = new elm.Bg(win["obj"], "/home/sanjeev/Pictures/computer.png");
print("Label test = " + win["title"]);
win["title"] = "Changing world";
win["autodel"] = true;
print("Label test = " + win["title"]);
bg["file"] = null;
bg["red"] = 255;