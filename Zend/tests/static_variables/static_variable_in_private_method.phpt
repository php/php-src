--TEST--
Inheritance of private method with static variable
--FILE--
<?php

class A {
    private function m() {
        static $x;
    }
}
class B extends A {}

?>
===DONE===
--EXPECT--
===DONE===
