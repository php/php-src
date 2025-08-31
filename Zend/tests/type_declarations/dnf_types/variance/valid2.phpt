--TEST--
Commutative intersection types
--FILE--
<?php

interface A {}
interface B {}
interface X {}

class Foo {
    public X|(A&B) $prop;
    public function foo(X|(A&B) $v): X|(A&B) {}
}

class FooChild extends Foo {
    public (B&A)|X $prop;
    public function foo((B&A)|X $v): (B&A)|X {}
}

?>
===DONE===
--EXPECT--
===DONE===
