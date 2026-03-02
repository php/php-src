--TEST--
Replacing union of classes respecting intersection type by intersection type
--FILE--
<?php

interface X {}
interface Y {}

class TestOne implements X, Y {}
class TestTwo implements X, Y {}

interface A
{
    public function foo(TestOne|TestTwo $param): X&Y;
}

interface B extends A
{
    public function foo(X&Y $param): TestOne|TestTwo;
}

interface C extends A
{
    public function foo(X $param): TestTwo;
}

interface D extends A
{
    public function foo(Y $param): TestOne;
}

interface E extends B
{
    public function foo(X $param): TestTwo;
}

interface F extends B
{
    public function foo(Y $param): TestOne;
}

?>
===DONE===
--EXPECT--
===DONE===
