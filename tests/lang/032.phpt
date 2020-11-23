--TEST--
Class method registration
--FILE--
<?php
class A {
    function foo() {}
}

class B extends A {
    function foo() {}
}

class C extends B {
    function foo() {}
}

class D extends A {
}

class F extends D {
    function foo() {}
}

// Following class definition should fail, but cannot test
/*
class X {
    function foo() {}
    function foo() {}
}
*/

echo "OK\n";
?>
--EXPECT--
OK
