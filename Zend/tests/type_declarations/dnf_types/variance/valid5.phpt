--TEST--
Replacing union of classes respecting intersection type by intersection type
--FILE--
<?php

interface X {}
interface Y {}

class TestOne implements X, Y {}
class TestTwo implements X, Y {}
class Foo {}

interface A
{
    public function foo(TestOne|TestTwo|Foo $param): (X&Y)|Foo;
}

interface B extends A
{
    public function foo((X&Y)|Foo $param): TestOne|TestTwo|Foo;
}

interface C extends A
{
    public function foo(X|Foo $param): TestTwo;
}

interface D extends A
{
    public function foo(Y|Foo $param): TestOne;
}

interface E extends B
{
    public function foo(X|Foo $param): TestTwo;
}

interface F extends B
{
    public function foo(Y|Foo $param): TestOne;
}

?>
===DONE===
--EXPECT--
===DONE===
