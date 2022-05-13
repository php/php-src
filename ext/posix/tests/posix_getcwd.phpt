--TEST--
posix_getcwd(): Basic tests
--EXTENSIONS--
posix
--SKIPIF--
<?php
if (!function_exists('posix_getcwd')) die('skip posix_getcwd() not found');
?>
--FILE--
<?php

var_dump(posix_getcwd());

?>
--EXPECTF--
string(%d) "%s"
