--TEST--
Do not leak when assigning to reference set with multiple typed properties with type coercion
--FILE--
<?php

class Test {
    public string $x;
    public string $y;
}

$test = new Test;
$ref = "";
$test->x =& $ref;
$test->y =& $ref;
$val = 42;
$ref = $val;
var_dump($ref, $val);

?>
--EXPECT--
string(2) "42"
int(42)
