--TEST--
Assignments to references that are held by properties with union types
--FILE--
<?php

class Test {
    public int|string $x;
    public float|string $y;
}

$test = new Test;
$r = "foobar";
$test->x =& $r;
$test->y =& $r;

$v = 42;
try {
    $r = $v;
} catch (TypeError $e) {
    echo $e->getMessage(), "\n";
}
var_dump($r, $v);

$v = 42.0;
try {
    $r = $v;
} catch (TypeError $e) {
    echo $e->getMessage(), "\n";
}
var_dump($r, $v);

unset($r, $test->x, $test->y);

$test->x = 42;
try {
    $test->y =& $test->x;
} catch (TypeError $e) {
    echo $e->getMessage(), "\n";
}

unset($test->x, $test->y);

$test->y = 42.0;
try {
    $test->x =& $test->y;
} catch (TypeError $e) {
    echo $e->getMessage(), "\n";
}

?>
--EXPECT--
Cannot assign int to reference held by property Test::$x of type string|int and property Test::$y of type string|float, as this would result in an inconsistent type conversion
string(6) "foobar"
int(42)
Cannot assign float to reference held by property Test::$x of type string|int and property Test::$y of type string|float, as this would result in an inconsistent type conversion
string(6) "foobar"
float(42)
Reference with value of type int held by property Test::$x of type string|int is not compatible with property Test::$y of type string|float
Reference with value of type float held by property Test::$y of type string|float is not compatible with property Test::$x of type string|int
