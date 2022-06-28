--TEST--
Interface default methods with abstract class inheritance
--FILE--
<?php

interface Interface1
{
    function method1() { echo __METHOD__, "\n"; }
}

abstract class AbstractClass1
{
    function method1() { echo __METHOD__, "\n"; }
}

class Class1
    extends AbstractClass1
    implements Interface1
{
}

(new Class1())->method1();

?>
--EXPECT--
AbstractClass1::method1

