--TEST--
posix_mknod(): Basic tests
--SKIPIF--
<?php
if (!extension_loaded('posix')) die('skip - POSIX extension not loaded');
if (!function_exists('posix_mknod')) die('skip posix_mknod() not found');
?>
--FILE--
<?php

var_dump(posix_mknod(NULL, NULL, NULL, NULL));

?>
--EXPECT--
bool(false)
