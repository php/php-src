--TEST--
Session Object Deserialization
--SKIPIF--
<?php include('skipif.inc'); ?>
--INI--
register_globals=1
--FILE--
<?php

class foo {
	var $bar = "ok";
	function method() { $this->yes++; }
}

session_id("abtest");
session_start();
session_decode('baz|O:3:"foo":2:{s:3:"bar";s:2:"ok";s:3:"yes";i:1;}arr|a:1:{i:3;O:3:"foo":2:{s:3:"bar";s:2:"ok";s:3:"yes";i:1;}}');

$baz->method();
$arr[3]->method();

var_dump($baz);
var_dump($arr);
session_destroy();
--EXPECT--
object(foo)(2) {
  ["bar"]=>
  string(2) "ok"
  ["yes"]=>
  int(2)
}
array(1) {
  [3]=>
  &object(foo)(2) {
    ["bar"]=>
    string(2) "ok"
    ["yes"]=>
    int(2)
  }
}
