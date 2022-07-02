--TEST--
Bug #72943 (assign_dim on string doesn't reset hval)
--FILE--
<?php
$array = array("test" => 1);

$a = "lest";
var_dump($array[$a]);
$a[0] = "f";
var_dump($array[$a]);
$a[0] = "t";
var_dump($array[$a]);
?>
--EXPECTF--
Warning: Undefined array key "lest" in %s on line %d
NULL

Warning: Undefined array key "fest" in %s on line %d
NULL
int(1)
