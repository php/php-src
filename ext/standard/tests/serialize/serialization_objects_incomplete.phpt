--TEST--
Check behaviour of incomplete class
--FILE--
<?php
$incomplete = unserialize('O:1:"C":1:{s:1:"p";i:1;}');
var_dump($incomplete);
?>
--EXPECT--
object(__PHP_Incomplete_Class)#1 (2) {
  ["__PHP_Incomplete_Class_Name"]=>
  string(1) "C"
  ["p"]=>
  int(1)
}
