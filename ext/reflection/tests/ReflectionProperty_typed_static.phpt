--TEST--
ReflectionProperty::getValue() on typed static property
--FILE--
<?php

class Test {
    public static int $x = 42;
    public static int $y;
}

$rp = new ReflectionProperty('Test', 'x');
var_dump($rp->getValue());

$rp = new ReflectionProperty('Test', 'y');
try {
    var_dump($rp->getValue());
} catch (TypeError $e) {
    echo $e->getMessage(), "\n";
}

?>
--EXPECT--
int(42)
Typed static property Test::$y must not be accessed before initialization
