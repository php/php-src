--TEST--
Co-variance failure for intersection type where child is union, but not all members are a subtype of intersection 2
--FILE--
<?php

interface X {}
interface Y {}
interface Z extends Y {}

class TestOne implements X, Z {}
class TestTwo implements X, Y {}

interface A
{
    public function foo(): X&Z;
}

interface B extends A
{
    public function foo(): TestOne|TestTwo;
}

?>
--EXPECTF--
Fatal error: Declaration of B::foo(): TestOne|TestTwo must be compatible with A::foo(): X&Z in %s on line %d
