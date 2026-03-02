--TEST--
Co-variance check failure for intersection type where child removes one of intersection type members
--FILE--
<?php

interface A {}
interface B {}
interface C {}

class Test implements A, B, C {}

class Foo {
    public function foo(): A&B&C {
        return new Test();
    }
}

/* This fails because just A&B larger than A&B&C */
class FooChild extends Foo {
    public function foo(): A&B {
        return new Test();
    }
}

?>
--EXPECTF--
Fatal error: Declaration of FooChild::foo(): A&B must be compatible with Foo::foo(): A&B&C in %s on line %d
