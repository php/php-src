--TEST--
session object serialization
--SKIPIF--
<?php include('skipif.inc'); ?>
--INI--
session.use_cookies=0
session.cache_limiter=
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
session_start();
$_SESSION["baz"] = $baz;
$_SESSION["arr"] = $arr;
var_dump(session_encode());
session_destroy();
?>
--EXPECT--
unicode(126) "baz|O:3:"foo":2:{U:3:"bar";U:2:"ok";U:3:"yes";U:4:"done";}arr|a:1:{i:3;O:3:"foo":2:{U:3:"bar";U:2:"ok";U:3:"yes";U:4:"done";}}"

