--TEST--
Bug #77381 (heap buffer overflow in multibyte match_at)
--EXTENSIONS--
mbstring
--SKIPIF--
<?php
if (!function_exists('mb_ereg')) die('skip mb_ereg() not available');
?>
--FILE--
<?php
var_dump(mb_ereg("000||0\xfa","0"));
var_dump(mb_ereg("(?i)000000000000000000000\xf0",""));
var_dump(mb_ereg("0000\\"."\xf5","0"));
var_dump(mb_ereg("(?i)FFF00000000000000000\xfd",""));
?>
--EXPECTF--
Warning: mb_ereg(): Pattern is not valid under UTF-8 encoding in %s on line %d
bool(false)

Warning: mb_ereg(): Pattern is not valid under UTF-8 encoding in %s on line %d
bool(false)

Warning: mb_ereg(): Pattern is not valid under UTF-8 encoding in %s on line %d
bool(false)

Warning: mb_ereg(): Pattern is not valid under UTF-8 encoding in %s on line %d
bool(false)
