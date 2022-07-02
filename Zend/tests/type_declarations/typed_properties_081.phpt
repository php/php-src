--TEST--
Clone must inherit typed references
--FILE--
<?php

class Test {
    public int $x = 42;
}

$test = new Test;
$x =& $test->x;
$test2 = clone $test;
unset($test);
try {
    $x = "foo";
} catch (TypeError $e) { echo $e->getMessage(), "\n"; }
var_dump($test2->x);

?>
--EXPECT--
Cannot assign string to reference held by property Test::$x of type int
int(42)
