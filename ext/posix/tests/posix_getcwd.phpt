--TEST--
posix_getcwd(): Basic tests
--SKIPIF--
<?php
if (!extension_loaded('posix')) die('skip - POSIX extension not loaded'); 
if (!function_exists('posix_getcwd')) die('skip posix_getcwd() not found');
?>
--FILE--
<?php

var_dump(posix_getcwd());
var_dump(posix_getcwd(1));

?>
--EXPECTF--
string(%d) "%s"

Warning: posix_getcwd() expects exactly 0 parameters, 1 given in %s on line %d
NULL
