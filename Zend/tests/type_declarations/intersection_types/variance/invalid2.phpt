--TEST--
Co-variance check failure for intersection type where child replaces it with standard type
--FILE--
<?php

interface A {}
interface B {}

class Test implements A, B {}

class Foo {
    public function foo(): A&B {
        return new Test();
    }
}

/* This fails because just A larger than A&B */
class FooChild extends Foo {
    public function foo(): array {
        return new Test();
    }
}

?>
--EXPECTF--
Fatal error: Declaration of FooChild::foo(): array must be compatible with Foo::foo(): A&B in %s on line %d
