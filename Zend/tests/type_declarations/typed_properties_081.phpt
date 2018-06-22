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
} catch (TypeError $e) { var_dump($e->getMessage()); }
var_dump($test2->x);

?>
--EXPECT--
string(45) "Cannot assign string to reference of type int"
int(42)

