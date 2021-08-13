--TEST--
Bug #75420.16 (Indirect modification of magic method argument)
--FILE--
<?php
class Test implements ArrayAccess {
    public function offsetExists($x): bool { }
    public function offsetGet($x): mixed { }
    public function offsetSet($x, $y): void { $GLOBALS["obj"] = 24; var_dump($this); }
    public function offsetUnset($x): void { }
}

$obj = new Test;
$name = "foo";
$obj[$name] = 1;
var_dump($obj);
?>
--EXPECT--
object(Test)#1 (0) {
}
int(24)
