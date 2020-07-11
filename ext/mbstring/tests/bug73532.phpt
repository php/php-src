--TEST--
Bug #73532 (Null pointer dereference in mb_eregi)
--SKIPIF--
<?php
require 'skipif.inc';
if (!function_exists('mb_ereg')) die('skip mbregex support not available');
?>
--FILE--
<?php
var_dump(mb_eregi("a", "\xf5"));
?>
--EXPECT--
bool(false)
