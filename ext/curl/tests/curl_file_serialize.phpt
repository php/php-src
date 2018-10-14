--TEST--
CURL file uploading
--SKIPIF--
<?php
if (!extension_loaded("curl")) {
	exit("skip curl extension not loaded");
}
?>
--FILE--
<?php
$data = 'a:2:{s:4:"file";O:8:"CURLFile":3:{s:4:"name";s:13:"testdata1.txt";s:4:"mime";s:0:"";s:8:"postname";s:0:"";}s:4:"data";s:3:"foo";}';
var_dump(unserialize($data));
?>
--EXPECTF--
Fatal error: Uncaught Exception: Unserialization of CURLFile instances is not allowed in %s
Stack trace:
#0 [internal function]: CURLFile->__wakeup()
#1 %s
#2 {main}
  thrown in %s on line %d
