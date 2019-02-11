--TEST--
Bug #72994 (mbc_to_code() out of bounds read)
--SKIPIF--
<?php
if (!extension_loaded('mbstring')) die('skip mbstring extension not available');
if (!function_exists('mbereg_replace')) die('skip mbereg_replace() not available');
?>
--FILE--
<?php
$var1 = mbereg_replace($var-232338951,NULL,NULL,NULL);
var_dump($var1);
?>
===DONE===
--EXPECTF--
Notice: Undefined variable: var in %s on line %d

Warning: mbereg_replace(): Pattern is not valid under UTF-8 encoding in %s on line %d
bool(false)
===DONE===
