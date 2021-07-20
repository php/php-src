--TEST--
Intersection type removing nullable
--FILE--
<?php

class A {}
class B extends A {}

class Test {
    public A&B|null $prop;
}
class Test2 extends Test {
    public A&B $prop;
}

?>
--EXPECTF--
Fatal error: Type of Test2::$prop must be A&B|null (as in class Test) in %s on line %d
