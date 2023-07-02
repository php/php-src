--TEST--
Interface default methods can't be referred to by parent by inheriter.
--FILE--
<?php

interface Interface1
{
    function method1() { echo __METHOD__, "\n"; }
}

class Class1 implements Interface1
{
    function method1() { parent::method1(); }
}

(new Class1())->method1();

?>
--EXPECTF--
Fatal error: Cannot use "parent" when current class scope has no parent in %s on line %d

