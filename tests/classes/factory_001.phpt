--TEST--
ZE2 factory objects
--FILE--
<?php

class Circle {
    function draw() {
        echo "Circle\n";
    }
}

class Square {
    function draw() {
        print "Square\n";
    }
}

function ShapeFactoryMethod($shape) {
    switch ($shape) {
        case "Circle":
            return new Circle();
        case "Square":
            return new Square();
    }
}

ShapeFactoryMethod("Circle")->draw();
ShapeFactoryMethod("Square")->draw();

?>
--EXPECT--
Circle
Square
