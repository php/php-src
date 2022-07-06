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

?>
--EXPECTF--
string(%d) "%s"
