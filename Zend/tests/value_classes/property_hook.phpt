--TEST--
Value classes inherit the restriction on readonly property hooks
--FILE--
<?php
value class Foo { public int $number { get => 42; } }
?>
--EXPECTF--
Fatal error: Hooked properties cannot be readonly in %s on line %d
