--TEST--
Bug #21600 (assign by reference function call changes variable contents)
--FILE--
<?php
$tmp = array();
$tmp['foo'] = "test";
$tmp['foo'] = &bar($tmp['foo']);
var_dump($tmp);

function bar($text){
  return $text;
}
?>
--EXPECT--
array(1) {
  ["foo"]=>
  string(4) "test"
}

