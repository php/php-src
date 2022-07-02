--TEST--
Invalid union type variance: Using parent of class in return type
--FILE--
<?php

class X {}
class Y extends X {}

class A {
    public function method(): Y|string {}
}
class B extends A {
    public function method(): X|string {}
}

?>
--EXPECTF--
Fatal error: Declaration of B::method(): X|string must be compatible with A::method(): Y|string in %s on line %d
