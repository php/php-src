--TEST--
Interface default methods are callable by `parent` in subclass of inheriter.
--FILE--
<?php

class Class0 {
    function method1() { echo __METHOD__, "\n"; }
}

class Class1 extends Class0 {
    // Method Interface1::method1() has been inherited here...
}

class Class2 extends Class1 {
    // ... so it's callable by `parent` here.
    function method1() { parent::method1(); }
}

(new Class1())->method1();

?>
--EXPECTF--
Interface1::method1
