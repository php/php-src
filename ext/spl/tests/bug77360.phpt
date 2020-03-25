--TEST--
Bug #77360: class_uses causes segfault
--FILE--
<?php

class foobar {}
$str = "foo";
$str .= "bar";
var_dump(class_uses($str, false));
var_dump(class_uses($str, false));
var_dump($str);

?>
--EXPECT--
array(0) {
}
array(0) {
}
string(6) "foobar"
