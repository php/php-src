--TEST--
Bug #77371 (heap buffer overflow in mb regex functions - compile_string_node)
--EXTENSIONS--
mbstring
--SKIPIF--
<?php
if (!function_exists('mb_ereg')) die('skip mb_ereg() not available');
?>
--FILE--
<?php
var_dump(mb_ereg("()0\xfc00000\xfc00000\xfc00000\xfc",""));
?>
--EXPECTF--
Deprecated: Function mb_ereg() is deprecated since 8.6, because the underlying library is no longer maintained in %s on line %d

Warning: mb_ereg(): Pattern is not valid under UTF-8 encoding in %s on line %d
bool(false)
