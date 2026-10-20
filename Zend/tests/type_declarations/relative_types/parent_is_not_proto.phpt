--TEST--
The parent class is not necessarily the class of the prototype
--FILE--
<?php

class A {
    function test(): B {}
}
class B extends A {}
class C extends B {
    function test(): parent {}
}

?>
===DONE===
--EXPECT--
===DONE===
