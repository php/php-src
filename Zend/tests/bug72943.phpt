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
Notice: Undefined index: lest in %sbug72943.php on line %d
NULL

Notice: Undefined index: fest in %sbug72943.php on line %d
NULL
int(1)
