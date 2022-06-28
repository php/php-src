--TEST--
Interface default methods
--FILE--
<?php

interface Interface1
{
    function method1() { echo __METHOD__, "\n"; }
}

class Class1
    implements Interface1
{
}

$object1 = new Class1();
$object1->method1();

?>
--EXPECT--
Interface1::method1

