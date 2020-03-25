--TEST--
Bug #75420.15 (Indirect modification of magic method argument)
--FILE--
<?php
class Test implements ArrayAccess {
    public function offsetExists($x) { }
    public function offsetGet($x) { }
    public function offsetSet($x, $y) { $GLOBALS["name"] = 24; var_dump($x); }
    public function offsetUnset($x) { }
}

$obj = new Test;
$name = "foo";
$name = str_repeat($name, 2);
$obj[$name] = 1;
var_dump($name);
?>
--EXPECT--
string(6) "foofoo"
int(24)
