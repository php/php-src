--TEST--
session object deserialization
--SKIPIF--
<?php include('skipif.inc'); ?>
--INI--
session.use_cookies=0
session.cache_limiter=
register_globals=1
session.serialize_handler=php
session.save_handler=files
--FILE--
<?php
error_reporting(E_ALL);

class foo {
	public $bar = "ok";
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
Warning: Directive 'register_globals' is deprecated in PHP 5.3 and greater in Unknown on line 0
object(foo)#1 (2) {
  ["bar"]=>
  string(2) "ok"
  ["yes"]=>
  int(2)
}
array(1) {
  [3]=>
  object(foo)#2 (2) {
    ["bar"]=>
    string(2) "ok"
    ["yes"]=>
    int(2)
  }
}

