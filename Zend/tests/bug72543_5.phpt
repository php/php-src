--TEST--
Bug #72543.5 (different references behavior comparing to PHP 5)
--FILE--
<?php
$arr = [1];
$ref =& $arr[0];
var_dump($arr[0] + ($arr[0] = 2));

$obj = new stdClass;
$obj->prop = 1;
$ref =& $obj->prop;
var_dump($obj->prop + ($obj->prop = 2));

?>
--EXPECT--
int(3)
int(3)
