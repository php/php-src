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

Warning: mb_ereg_replace(): mbregex compile err: invalid code point value in %sbug72994.php on line %d
bool(false)
===DONE===
