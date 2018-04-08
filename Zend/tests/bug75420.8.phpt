--TEST--
Bug #75420.8 (Indirect modification of magic method argument)
--FILE--
<?php
class Test {
	public function __set($x,$v) { $GLOBALS["obj"] = 24; var_dump($this); }
}

$obj = new Test;
$name = "foo";
$obj->$name = 1;
var_dump($obj);
?>
--EXPECT--
object(Test)#1 (0) {
}
int(24)
