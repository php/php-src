--TEST--
Cleanup of shared static variables HT that has never been used should not assert
--FILE--
<?php

class A {
    public function test() {
        static $x;
    }
}
class B extends A {}

?>
===DONE===
--EXPECT--
===DONE===
