--TEST--
Bug #72994 (mbc_to_code() out of bounds read)
--SKIPIF--
<?php
if (!extension_loaded('mbstring')) die('skip mbstring extension not available');
if (!function_exists('mb_ereg_replace')) die('skip mb_ereg_replace() not available');
?>
--FILE--
<?php
$var1 = mb_ereg_replace($var-232338951,NULL,NULL,NULL);
var_dump($var1);
?>
===DONE===
--EXPECTF--
Notice: Undefined variable: var in %s on line %d

Deprecated: mb_ereg_replace(): Non-string patterns will be interpreted as strings in the future. Use an explicit chr() call to preserve the current behavior in %s on line %d

Warning: mb_ereg_replace(): Pattern is not valid under UTF-8 encoding in %s on line %d
bool(false)
===DONE===
