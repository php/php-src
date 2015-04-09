--TEST--
Bug #21600 (assign by reference function call changes variable contents)
--INI--
error_reporting=4095
--FILE--
<?php
$tmp = array();
$tmp['foo'] = "test";
$tmp['foo'] = &bar($tmp['foo']);
var_dump($tmp);

unset($tmp);

$tmp = array();
$tmp['foo'] = "test";
$tmp['foo'] = &fubar($tmp['foo']);
var_dump($tmp);

function bar($text){
  return $text;
}

function fubar($text){
  $text = &$text;
  return $text;
}
?>
--EXPECTF--
Strict Standards: Only variables should be assigned by reference in %sbug21600.php on line 4
array(1) {
  ["foo"]=>
  string(4) "test"
}

Strict Standards: Only variables should be assigned by reference in %sbug21600.php on line 11
array(1) {
  ["foo"]=>
  string(4) "test"
}
