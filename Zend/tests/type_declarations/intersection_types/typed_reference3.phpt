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
    public ?X&Y $y;
    public ?X&Z $z;
}
$test = new Test;
$r = new A;
$test->y =& $r;
$test->z =& $r;

$r = null;

?>
==DONE==
--EXPECT--
==DONE==
