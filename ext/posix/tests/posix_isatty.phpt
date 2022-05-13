--TEST--
posix_isatty(): Basic tests
--EXTENSIONS--
posix
--SKIPIF--
<?php
if (!function_exists('posix_isatty')) die('skip posix_isatty() not found');
?>
--FILE--
<?php

var_dump(posix_isatty(0));

?>
--EXPECTF--
bool(%s)
