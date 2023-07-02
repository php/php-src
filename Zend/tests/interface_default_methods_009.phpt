--TEST--
Interface default methods can be cancelled
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
    implements Interface2
{
}

(new Class1())->method1();

?>
--EXPECTF--
Fatal error: Class Class1 contains 1 abstract method and must therefore be declared abstract or implement the remaining methods (Interface2::method1) in %s on line %d

