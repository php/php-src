--TEST--
Interface default methods with conflicting interface inheritance
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
}

?>
--EXPECTF--
Fatal error: Type Class1 already has default method Interface1::method1(); cannot override it with another from Interface2 in %s on line %d
