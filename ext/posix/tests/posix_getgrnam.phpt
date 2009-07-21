--TEST--
posix_getgrnam(): Basic tests
--SKIPIF--
<?php
if (!extension_loaded('posix')) die('skip - POSIX extension not loaded'); 
if (!function_exists('posix_getgrnam')) die('skip posix_getgrnam() not found');
?>
--FILE--
<?php

var_dump(posix_getgrnam(NULL));
var_dump(posix_getgrnam(1));
var_dump(posix_getgrnam(''));

?>
--EXPECT--
bool(false)
bool(false)
bool(false)
