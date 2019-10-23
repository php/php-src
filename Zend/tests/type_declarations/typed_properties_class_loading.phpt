--TEST--
Typed properties do not invoke the autoloader
--FILE--
<?php

class Test {
    public X $propX;
    public ?Y $propY;
}

spl_autoload_register(function($class) {
    echo "Loading $class\n";
});

$test = new Test;
try {
    $test->propX = new stdClass;
} catch (TypeError $e) {
    echo $e->getMessage(), "\n";
}

if (true) {
    class X {}
}

$test->propX = new X;
var_dump($test->propX);

$test->propY = null;
$r =& $test->propY;
try {
    $test->propY = new stdClass;
} catch (TypeError $e) {
    echo $e->getMessage(), "\n";
}

if (true) {
    class Y {}
}

$r = new Y;
var_dump($test->propY);

?>
--EXPECT--
Typed property Test::$propX must be an instance of X, stdClass used
object(X)#3 (0) {
}
Typed property Test::$propY must be an instance of Y or null, stdClass used
object(Y)#4 (0) {
}
