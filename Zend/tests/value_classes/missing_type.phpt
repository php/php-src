--TEST--
Value class properties require a declared type
--FILE--
<?php
value class Foo { public $bar; }
?>
--EXPECTF--
Fatal error: Readonly property Foo::$bar must have type in %s on line %d
