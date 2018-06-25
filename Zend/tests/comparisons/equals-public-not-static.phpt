--TEST--
__equals: Magic method must be public and non-static
--FILE--
<?php

class A {
    private function __equals() {}
}

class B {
    protected function __equals() {}
}

class C {
    static public function __equals() {}
}

?>
--EXPECTF--
Warning: The magic method __equals() must have public visibility and cannot be static in %s on line %d

Warning: The magic method __equals() must have public visibility and cannot be static in %s on line %d

Warning: The magic method __equals() must have public visibility and cannot be static in %s on line %d
