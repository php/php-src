--TEST--
Typed property invariance violation for protected properties
--FILE--
<?php

class A { protected int $x; }
class B extends A { protected $x; }

?>
--EXPECTF--
Fatal error: Type of B::$x must be int (as in class A) in %s on line %d
