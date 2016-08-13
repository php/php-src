--TEST--
Bug #70395 (Missing ARG_INFO for openssl_seal())
--SKIPIF--
<?php
if (!extension_loaded("openssl")) die("skip openssl not loaded");
?>
--FILE--
<?php
$func = new ReflectionFunction('openssl_seal');
$param = $func->getParameters()[4];
var_dump($param);
var_dump($param->isOptional());
?>
--EXPECTF--
object(ReflectionParameter)#%d (1) {
  ["name"]=>
  string(6) "method"
}
bool(true)
