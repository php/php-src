--TEST--
abstract private functions in traits (no concrete method error)
--FILE--
<?php

trait T {
    abstract private function f();
}

abstract class C {
    use T;
}

class D extends C {
}

?>
--EXPECTF--
Fatal error: Class D contains 1 abstract method and must therefore be declared abstract or implement the remaining methods (C::f) in %s on line %d
