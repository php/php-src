--TEST--
Co-variance check failure for intersection type where child removes one of intersection type members
--FILE--
<?php

interface A {}
interface B {}
interface X {}

class Test implements A, B {}

class Foo {
    public function foo(): (A&B)|X {
        return new Test();
    }
}

/* This fails because just A larger than A&B */
class FooChild extends Foo {
    public function foo(): A|X {
        return new Test();
    }
}

?>
--EXPECTF--
Fatal error: Declaration of FooChild::foo(): A|X must be compatible with Foo::foo(): (A&B)|X in %s on line %d
