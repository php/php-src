--TEST--
Bug #52355 (Negating zero does not produce negative zero)
--FILE--
<?php

var_dump(-0.0);
var_dump(-(float)"0");

$foo = -sin(0);

var_dump($foo);

try {
    var_dump(1.0 / -0.0);
} catch (\DivisionByZeroError $e) {
    echo $e->getMessage() . \PHP_EOL;
}

?>
--EXPECT--
float(-0)
float(-0)
float(-0)
Division by zero
