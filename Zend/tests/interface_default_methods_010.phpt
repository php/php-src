--TEST--
Interface default methods can be cancelled, but not if both interfaces are implemented.
--FILE--
<?php

interface Interface1
{
    function method1() { echo __METHOD__, "\n"; }
}

interface Interface2
    extends Interface1
{
    /* This will forbid inheriting Interface1::method1 if an implementer
     * implements Interface2 (not Interface1).
     */
    function method1();
}

class Class1
    implements Interface1, Interface2
{
}

(new Class1())->method1();

?>
--EXPECT--
Interface1::method1
