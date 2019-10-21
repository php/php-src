--TEST--
Class loading issues related to multiple class types in a union
--FILE--
<?php

class Test {
    public X|Y|Z|int $prop;
}

spl_autoload_register(function($class) {
    echo "Loading $class\n";
});

$test = new Test;

// Should not cause class loading, as not an object
$test->prop = 42; 
var_dump($test->prop);

// Should try to load both classes
try {
    $test->prop = new stdClass;
} catch (TypeError $e) {
    echo $e->getMessage(), "\n";
}

if (true) {
    class X {}
}

// Should not cause class loading, as X is already loaded
try {
    $test->prop = new X;
} catch (TypeError $e) {
    echo $e->getMessage(), "\n";
}
var_dump($test->prop);

if (true) {
    class Z {}
}

// TODO: Should this load class Y or not?
try {
    $test->prop = new Z;
} catch (TypeError $e) {
    echo $e->getMessage(), "\n";
}
var_dump($test->prop);

?>
--EXPECT--
int(42)
Loading X
Loading Y
Loading Z
Cannot assign stdClass to property Test::$prop of type X|Y|Z|int
object(X)#3 (0) {
}
Loading Y
object(Z)#5 (0) {
}
