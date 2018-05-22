--TEST--
Bug #75307 Wrong reflection for openssl_open function
--SKIPIF--
<?php
if (!extension_loaded("openssl")) die("skip openssl not available");
?>
--FILE--
<?php
$rf = new ReflectionFunction('openssl_open');
var_dump($rf->getNumberOfParameters());
var_dump($rf->getNumberOfRequiredParameters());
?>
===DONE===
--EXPECT--
int(6)
int(4)
===DONE===
