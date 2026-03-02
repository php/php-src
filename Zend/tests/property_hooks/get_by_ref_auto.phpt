--TEST--
Get by reference with hooked property
--FILE--
<?php

class Test {
    public $byVal { get { return []; } }
}

$test = new Test;

try {
    $test->byVal[] = 42;
} catch (\Error $e) {
    echo get_class($e) . ': ' . $e->getMessage() . "\n";
}
var_dump($test->byVal);

try {
    $test->byVal =& $ref;
} catch (Error $e) {
    echo get_class($e) . ': ' . $e->getMessage() . "\n";
}

?>
--EXPECT--
Error: Indirect modification of Test::$byVal is not allowed
array(0) {
}
Error: Cannot assign by reference to overloaded object
