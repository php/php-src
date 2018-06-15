--TEST--
Bug #75420.13 (Indirect modification of magic method argument)
--FILE--
<?php
class Test implements ArrayAccess {
	public function offsetExists($x) { $GLOBALS["obj"] = 24; return true; }
	public function offsetGet($x) { var_dump($x); return 42; }
	public function offsetSet($x, $y) { }
	public function offsetUnset($x) { }
}

$obj = new Test;
$name = "foo";
var_dump($obj[$name] ?? 12);
var_dump($obj);
?>
--EXPECT--
string(3) "foo"
int(42)
int(24)
