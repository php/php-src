--TEST--
Bug #62814: It is possible to stiffen child class members visibility
--FILE--
<?php

class A {
    private function test() { }
}

class B extends A {
    protected function test() { }
}

class C extends B {
    private function test() { }
}

?>
--EXPECTF--
Fatal error: Access level to C::test() must be protected (as in class B) or weaker in %s on line %d
