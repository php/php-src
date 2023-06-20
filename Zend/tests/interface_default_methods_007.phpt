--TEST--
Interface default methods are lower priority than trait methods
--FILE--
<?php

interface Interface1
{
    function method1() { echo __METHOD__, "\n"; }
}

trait Trait1
{
    function method1() { echo __METHOD__, "\n"; }
}

class Class1
    implements Interface1
{
    use Trait1;
}

(new Class1())->method1();

?>
--EXPECT--
Trait1::method1

