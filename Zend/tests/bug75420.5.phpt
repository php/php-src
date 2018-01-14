--TEST--
Bug #75420.5 (Indirect modification of magic method argument)
--FILE--
<?php
class Test {
	public function __isset($x) { $GLOBALS["obj"] = 24; return true; }
	public function __get($x) { var_dump($this); return 42; }
}

$obj = new Test;
$name = "foo";
var_dump($obj->$name ?? 12);
var_dump($obj);
?>
--EXPECT--
object(Test)#1 (0) {
}
int(42)
int(24)
