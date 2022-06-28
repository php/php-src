--TEST--
Interface default methods with interface inheritance
--FILE--
<?php

interface Interface1
{
    function method1() { echo __METHOD__, "\n"; }
}

interface Interface2
    extends Interface1
{
    function method1() { echo __METHOD__, "\n"; }
}

class Class1
    implements Interface2
{
}

$object1 = new Class1();
$object1->method1();

?>
--EXPECT--
Interface2::method1

