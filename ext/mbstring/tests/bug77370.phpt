--TEST--
Bug #77370 (Buffer overflow on mb regex functions - fetch_token)
--EXTENSIONS--
mbstring
--SKIPIF--
<?php
if (!function_exists('mb_split')) die('skip mb_split() not available');
?>
--FILE--
<?php
var_dump(mb_split("   \xfd",""));
?>
--EXPECTF--
Deprecated: Function mb_split() is deprecated since 8.6, because the underlying library is no longer maintained in %s on line 2

Warning: mb_split(): Pattern is not valid under UTF-8 encoding in %s on line %d
bool(false)
