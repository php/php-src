--TEST--
Typed property invariance violation for protected properties
--FILE--
<?php

class A { protected int $x; }
class B extends A { protected $x; }

?>
--EXPECTF--
Fatal error: Property B::$x must be of type int to be compatible with overridden property A::$x in %s on line %d
