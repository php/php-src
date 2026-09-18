--TEST--
Value classes cannot declare static properties
--FILE--
<?php
value class Foo { public static int $number; }
?>
--EXPECTF--
Fatal error: Static property Foo::$number cannot be readonly in %s on line %d
