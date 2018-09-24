--TEST--
Using settype() on a typed property
--FILE--
<?php

class Test {
    public int $x;
}

$test = new Test;
$test->x = 42;
settype($test->x, 'string');
// Same as $test->x = (string) $test->x.
// Leaves value unchanged due to coercion
var_dump($test->x);

try {
    settype($test->x, 'array');
} catch (TypeError $e) {
    echo $e->getMessage(), "\n";
}
var_dump($test->x);

?>
--EXPECT--
int(42)
Cannot assign array to reference held by property Test::$x of type int
int(42)
