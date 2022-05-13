--TEST--
Co-variance check failure for intersection type where child removes one of intersection type members
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
    public function foo(): A {
        return new Test();
    }
}

?>
--EXPECTF--
Fatal error: Declaration of FooChild::foo(): A must be compatible with Foo::foo(): A&B in %s on line %d
