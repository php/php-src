--TEST--
Bug #75420.3 (Indirect modification of magic method argument)
--FILE--
<?php
class Test {
    public function __isset($x) { $GLOBALS["name"] = 24; return true; }
    public function __get($x) { var_dump($x); return 42; }
}

$obj = new Test;
$name = "foo";
var_dump(empty($obj->$name));
var_dump($name);
?>
--EXPECT--
string(3) "foo"
bool(false)
int(24)
