--TEST--
ReflectionProperty::getValue() on typed static property
--FILE--
<?php

class Test {
    public static int $x = 42;
    public static int $y;
    public static $z;
}

$rp = new ReflectionProperty('Test', 'x');
var_dump($rp->getValue());

$rp = new ReflectionProperty('Test', 'y');
try {
    var_dump($rp->getValue());
} catch (Error $e) {
    echo $e->getMessage(), "\n";
}

$rp->setValue(null, "24");
var_dump($rp->getValue());

try {
    $rp->setValue(null, "foo");
} catch (TypeError $e) {
    echo $e->getMessage(), "\n";
}
var_dump($rp->getValue());

Test::$z =& Test::$y;

$rp = new ReflectionProperty('Test', 'z');
try {
    $rp->setValue(null, "foo");
} catch (TypeError $e) {
    echo $e->getMessage(), "\n";
}
var_dump($rp->getValue());


?>
--EXPECT--
int(42)
Typed static property Test::$y must not be accessed before initialization
int(24)
Cannot assign string to property Test::$y of type int
int(24)
Cannot assign string to reference held by property Test::$y of type int
int(24)
