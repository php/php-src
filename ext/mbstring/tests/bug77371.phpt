--TEST--
Bug #77371 (heap buffer overflow in mb regex functions - compile_string_node)
--SKIPIF--
<?php
if (!extension_loaded('mbstring')) die('skip mbstring extension not available');
if (!function_exists('mb_ereg')) die('skip mb_ereg() not available');
?>
--FILE--
<?php
var_dump(mb_ereg("()0\xfc00000\xfc00000\xfc00000\xfc",""));
?>
--EXPECTF--
Warning: mb_ereg(): Pattern is not valid under UTF-8 encoding in %s on line %d
bool(false)
