--TEST--
Bug #25378 (unserialize() crashes with invalid data)
--FILE--
<?php
var_dump(unserialize('b:0;'));
var_dump(unserialize('b:1;'));
var_dump(unserialize('i:823;'));
var_dump(unserialize('s:0:"";'));
var_dump(unserialize('s:3:"foo";'));
var_dump(unserialize('a:1:{i:0;s:2:"12";}'));
var_dump(unserialize('a:2:{i:0;a:0:{}i:1;a:0:{}}'));
var_dump(unserialize('a:3:{i:0;s:3:"foo";i:1;s:3:"bar";i:2;s:3:"baz";}'));
var_dump(unserialize('O:8:"stdClass":0:{}'));
?>
===DONE===
--EXPECTF--
bool(false)
bool(true)
int(823)
string(0) ""
string(3) "foo"
array(1) {
  [0]=>
  string(2) "12"
}
array(2) {
  [0]=>
  array(0) {
  }
  [1]=>
  array(0) {
  }
}
array(3) {
  [0]=>
  string(3) "foo"
  [1]=>
  string(3) "bar"
  [2]=>
  string(3) "baz"
}
object(stdClass)#%d (0) {
}
===DONE===
