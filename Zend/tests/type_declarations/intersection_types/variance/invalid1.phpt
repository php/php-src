--TEST--
Co-variance check failure for intersection type where child replace one of intersection type members with a supertype
--FILE--
<?php

interface A {}
interface B extends A {}
interface C {}

class Test implements B, C {}

class Foo {
    public function foo(): B&C {
        return new Test();
    }
}

/* This fails because A is a parent type for B */
class FooChild extends Foo {
    public function foo(): A&C {
        return new Test();
    }
}

?>
--EXPECTF--
Fatal error: Declaration of FooChild::foo(): A&C must be compatible with Foo::foo(): B&C in %s on line %d
