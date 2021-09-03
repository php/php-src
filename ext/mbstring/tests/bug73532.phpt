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
--EXPECT--
bool(false)
