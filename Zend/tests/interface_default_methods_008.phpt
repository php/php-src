--TEST--
Interface default methods are inherited even indirectly
--FILE--
<?php

interface Interface1
{
    function method1() { echo __METHOD__, "\n"; }
}

interface Interface2
    extends Interface1
{
}

class Class1
    implements Interface2
{
}

(new Class1())->method1();

?>
--EXPECT--
Interface1::method1

