--TEST--
Interface default methods with conflicting interface inheritance but concrete takes precedence
--FILE--
<?php

interface Interface1
{
    function method1() { echo __METHOD__, "\n"; }
}

interface Interface2
{
    function method1() { echo __METHOD__, "\n"; }
}

class Class1
    implements Interface1, Interface2
{
    function method1() { echo __METHOD__, "\n"; }
}

(new Class1())->method1();

?>
--EXPECTF--
Class1::method1

