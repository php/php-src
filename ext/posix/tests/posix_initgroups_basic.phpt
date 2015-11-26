--TEST--
posix_initgroups(): Basic tests
--SKIPIF--
<?php
if (!extension_loaded('posix')) die('skip - POSIX extension not loaded'); 
if (!function_exists('posix_initgroups')) die('skip posix_initgroups() not found');
?>
--FILE--
<?php
echo "Basic test of POSIX posix_initgroups function\n"; 
var_dump(posix_initgroups('foo', 'bar'));
var_dump(posix_initgroups(NULL, NULL));

?>
===DONE====
--EXPECTF--
Basic test of POSIX posix_initgroups function

Warning: posix_initgroups() expects parameter 2 to be integer, string given in %s on line %d
bool(false)
bool(false)
===DONE====
