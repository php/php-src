--TEST--
Bug #77370 (Buffer overflow on mb regex functions - fetch_token)
--SKIPIF--
<?php
if (!extension_loaded('mbstring')) die('skip mbstring extension not available');
if (!function_exists('mb_split')) die('skip mb_split() not available');
?>
--FILE--
<?php
var_dump(mb_split("   \xfd",""));
?>
--EXPECTF--
Warning: mb_split(): Pattern is not valid under UTF-8 encoding in %s on line %d
bool(false)
