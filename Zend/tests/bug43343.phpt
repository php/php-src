--TEST--
Bug #43343 (Variable class name)
--FILE--
<?php
class if {
	public static $bar = "if";
}
$foo = 'bar';
var_dump(new if::$$foo);
?>
--EXPECTF--
object(if)#1 (0) {
}
