--TEST--
Invalid nullable narrowing
--FILE--
<?php

interface A {}
interface B {}

class Foo {
    public function foo(?A&B $foo) {
    }
}

class FooChild extends Foo {
    public function foo(A&B $foo) {
    }
}

?>
--EXPECTF--
Fatal error: Declaration of FooChild::foo(A&B $foo) must be compatible with Foo::foo(?A&B $foo) in %s on line %d
