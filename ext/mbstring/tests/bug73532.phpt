--TEST--
Bug #73532 (Null pointer dereference in mb_eregi)
--EXTENSIONS--
mbstring
--SKIPIF--
<?php
if (!function_exists('mb_ereg')) die('skip mbregex support not available');
?>
--FILE--
<?php
var_dump(mb_eregi("a", "\xf5"));
?>
--EXPECTF--
Deprecated: Function mb_eregi() is deprecated since 8.6, because the underlying library is no longer maintained in %s on line %d
bool(false)
