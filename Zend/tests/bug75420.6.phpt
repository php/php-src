--TEST--
Bug #75420.6 (Indirect modification of magic method argument)
--FILE--
<?php
class Test {
    public function __isset($x) { $GLOBALS["obj"] = 24; return true; }
    public function __get($x) { var_dump($this); return 42; }
}

$obj = new Test;
$name = "foo";
var_dump(empty($obj->$name));
var_dump($obj);
?>
--EXPECT--
object(Test)#1 (0) {
}
bool(false)
int(24)
