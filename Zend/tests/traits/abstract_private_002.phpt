--TEST--
abstract private functions in traits (no concrete method error)
--FILE--
<?php

trait T {
    abstract private function f();
}

class C {
    use T;
}

?>
--EXPECTF--
Fatal error: Class C contains 1 abstract method and must therefore be declared abstract or implement the remaining methods (C::f) in %s on line %d
