--TEST--
Invalid nullable widening
--FILE--
<?php

interface A {}
interface B {}
interface C {}

class Test implements A, B, C {}

class Foo {
    public function foo(): A {
        return new Test();
    }
}

class FooChild extends Foo {
    public function foo(): A&B {
        return new Test();
    }
}

class FooSecondChild extends FooChild {
    public function foo(): ?A&B {
        return new Test();
    }
}

?>
--EXPECTF--
Fatal error: Declaration of FooSecondChild::foo(): ?A&B must be compatible with FooChild::foo(): A&B in %s on line %d
