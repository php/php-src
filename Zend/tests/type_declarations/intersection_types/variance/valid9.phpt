--TEST--
Valid inheritence - co-variance
--FILE--
<?php

interface A {}
interface B {}
interface C {}

class Test implements A, B, C {}

class Foo {
    public function foo(): A&B|null {
        return null;
    }
}

class FooChild extends Foo {
    public function foo(): A&B {
        return new Test();
    }
}

$o = new Foo();
var_dump($o->foo());
$o = new FooChild();
var_dump($o->foo());

?>
--EXPECTF--
NULL
object(Test)#%d (0) {
}
