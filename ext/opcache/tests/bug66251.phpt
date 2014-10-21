--TEST--
Bug #66251 (Constants get statically bound at compile time when Optimized)
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.optimization_level=-1
--SKIPIF--
<?php if (!extension_loaded('Zend OPcache') || php_sapi_name() != "cli") die("skip CLI only"); ?>
--FILE--
<?php
printf ("A=%s\n", getA());
const A="hello";
function getA() {return A;}
?>
--EXPECTF--
Notice: Use of undefined constant A - assumed 'A' in %sbug66251.php on line 4
A=A
