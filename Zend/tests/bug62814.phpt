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
Fatal error: Method C::test() must have protected or public visibility to be compatible with overridden method B::test() in %s on line %d
