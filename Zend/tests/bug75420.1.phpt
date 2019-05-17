--TEST--
Bug #75420.1 (Indirect modification of magic method argument)
--FILE--
<?php
class Test {
	public function __isset($x) { $GLOBALS["name"] = 24; return true; }
	public function __get($x) { var_dump($x); return 42; }
}

$obj = new Test;
$name = "foo";
var_dump($obj->$name ?? 12);
var_dump($name);
?>
--EXPECT--
string(3) "foo"
int(42)
int(24)
