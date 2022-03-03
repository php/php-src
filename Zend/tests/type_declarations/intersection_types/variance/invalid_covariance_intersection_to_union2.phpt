--TEST--
Co-variance failure for intersection type where child is union, but not all members are a subtype of intersection 2
--FILE--
<?php

interface X {}
interface Y {}

class TestOne implements X, Y {}

interface A
{
    public function foo(): X&Y;
}

interface B extends A
{
    public function foo(): TestOne|int;
}

?>
--EXPECTF--
Fatal error: Declaration of B::foo(): TestOne|int must be compatible with A::foo(): X&Y in %s on line %d
