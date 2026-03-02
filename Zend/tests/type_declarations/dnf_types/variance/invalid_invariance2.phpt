--TEST--
Intersection type reduction invalid invariant type check
--FILE--
<?php

interface X {}
interface Y {}
class A implements X, Y {}
class B {}

class Test {
    public (X&Y)|B $prop;
}
class Test2 extends Test {
    public A|B $prop;
}

?>
===DONE===
--EXPECTF--
Fatal error: Type of Test2::$prop must be (X&Y)|B (as in class Test) in %s on line %d
