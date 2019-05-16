--TEST--
Nullable typed property
--FILE--
<?php
class Foo {
	public int $foo = null;
}
?>
--EXPECTF--
Fatal error: Default value for property of type int may not be null. Use the nullable type ?int to allow null default value in %s on line %d
