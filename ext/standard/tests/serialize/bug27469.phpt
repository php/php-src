--TEST--
Bug #27469 (serialize() objects of incomplete class)
--FILE--
<?php
$str = 'O:9:"TestClass":0:{}';
$obj = unserialize($str);
var_dump($obj);
echo serialize($obj)."\n";
var_dump($obj);
echo serialize($obj)."\n";
var_dump($obj);
?>
--EXPECT--
object(__PHP_Incomplete_Class)#1 (1) {
  ["__PHP_Incomplete_Class_Name"]=>
  string(9) "TestClass"
}
O:9:"TestClass":0:{}
object(__PHP_Incomplete_Class)#1 (1) {
  ["__PHP_Incomplete_Class_Name"]=>
  string(9) "TestClass"
}
O:9:"TestClass":0:{}
object(__PHP_Incomplete_Class)#1 (1) {
  ["__PHP_Incomplete_Class_Name"]=>
  string(9) "TestClass"
}
