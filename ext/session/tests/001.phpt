--TEST--
session object serialization
--SKIPIF--
<?php include('skipif.inc'); ?>
--INI--
session.use_cookies=0
session.cache_limiter=
register_globals=1
--FILE--
<?php
error_reporting(E_ALL);

class foo {
	var $bar = "ok";

	function method() { $this->yes = "done"; }
}

$baz = new foo;
$baz->method();

$arr[3] = new foo;
$arr[3]->method();

session_register("baz");
session_register("arr");

print session_encode()."\n";

session_destroy();
--GET--
--POST--
--EXPECT--
baz|O:3:"foo":2:{s:3:"bar";s:2:"ok";s:3:"yes";s:4:"done";}arr|a:1:{i:3;O:3:"foo":2:{s:3:"bar";s:2:"ok";s:3:"yes";s:4:"done";}}
