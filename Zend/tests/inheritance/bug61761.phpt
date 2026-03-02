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
==DONE==
--EXPECT--
==DONE==
