--TEST--
Bug #43343 (Variable class name)
--FILE--
<?php
class namespace {
	public static $bar = "namespace";
}
$foo = 'bar';
var_dump(new namespace::$$foo);
?>
--EXPECTF--
object(namespace)#1 (0) {
}
