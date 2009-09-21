--TEST--
session object serialization
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
--EXPECTF--
Warning: Directive 'register_globals' is deprecated in PHP 5.3 and greater in Unknown on line 0

Deprecated: Function session_register() is deprecated in %s on line %d

Deprecated: Function session_register() is deprecated in %s on line %d
baz|O:3:"foo":2:{s:3:"bar";s:2:"ok";s:3:"yes";s:4:"done";}arr|a:1:{i:3;O:3:"foo":2:{s:3:"bar";s:2:"ok";s:3:"yes";s:4:"done";}}

