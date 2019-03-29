--TEST--
Bug #73532 (Null pointer dereference in mb_eregi)
--SKIPIF--
<?php
if (!extension_loaded('mbstring')) die('skip mbstring extension not available');
if (!function_exists('mb_eregi')) die('skip mb_eregi() not available');
?>
--FILE--
<?php
var_dump(mb_eregi("a", "\xf5"));
?>
--EXPECTF--
bool(false)
