--TEST--
Intersection types and typed reference
--FILE--
<?php

interface X {}
interface Y {}
interface Z {}

class A implements X, Y, Z {}
class B implements X, Y {}

class Test {
    public X&Y|null $y;
    public X&Z $z;
}
$test = new Test;
$r = new A;
$test->y =& $r;
$test->z =& $r;


try {
    $r = null;
} catch (\TypeError $e) {
    echo $e->getMessage(), \PHP_EOL;
}

?>
--EXPECT--
Cannot assign null to reference held by property Test::$z of type X&Z
