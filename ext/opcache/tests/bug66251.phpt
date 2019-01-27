--TEST--
Bug #66251 (Constants get statically bound at compile time when Optimized)
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.optimization_level=-1
--SKIPIF--
<?php if (!extension_loaded('Zend OPcache')) die("skip Zend OPcache extension not loaded"); ?>
--FILE--
<?php
printf ("A=%s\n", getA());
const A="hello";
function getA() {return A;}
?>
--EXPECTF--
Warning: Use of undefined constant A - assumed 'A' (this will throw an Error in a future version of PHP) in %sbug66251.php on line 4
A=A
