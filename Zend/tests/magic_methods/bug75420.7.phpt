--TEST--
Bug #75420.7 (Indirect modification of magic method argument)
--FILE--
<?php
class Test {
    public function __set($x,$v) { $GLOBALS["name"] = 24; var_dump($x); }
}

$obj = new Test;
$name = "foo";
$name = str_repeat($name, 2);
$obj->$name = 1;
var_dump($name);
?>
--EXPECT--
string(6) "foofoo"
int(24)
