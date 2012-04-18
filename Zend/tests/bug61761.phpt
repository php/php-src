--TEST--
Bug #61761 ('Overriding' a private static method with a different signature causes crash)
--FILE--
<?php

class A
{
        private static function test($a) { }
}

class B extends A
{
        private static function test($a, $b) { }
}

?>
--EXPECTF--
Strict Standards: Declaration of B::test() should be compatible with A::test($a) in %sbug61761.php on line %d
