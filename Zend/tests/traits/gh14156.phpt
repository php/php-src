--TEST--
GH-14156 (Inherited private method does not satisfy abstract trait requirement)
--FILE--
<?php
trait T {
    public abstract function test(): void;
}

class P {
    private function test(): void {}
}

class C extends P {
    use T;
}
?>
--EXPECTF--
Fatal error: Class C contains 1 abstract method and must therefore be declared abstract or implement the remaining method (C::test) in %s on line %d
