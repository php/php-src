--TEST--
Bug #78633 (Heap buffer overflow (read) in mb_eregi)
--SKIPIF--
<?php
if (!extension_loaded('mbstring')) die('skip mbstring extension not available');
if (!function_exists('mb_eregi')) die('skip mb_eregi function not available');
?>
--FILE--
<?php
var_dump(mb_eregi(".+Isssǰ", ".+Isssǰ"));
?>
--EXPECT--
bool(false)
