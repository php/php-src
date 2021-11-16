--TEST--
Commutative intersection types
--FILE--
<?php

interface A {}
interface B {}

class Foo {
    public A&B $prop;
    public function foo(A&B $v): A&B {}
}

class FooChild extends Foo {
    public B&A $prop;
    public function foo(B&A $v): B&A {}
}

?>
===DONE===
--EXPECT--
===DONE===
