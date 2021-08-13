--TEST--
posix_initgroups(): Basic tests
--EXTENSIONS--
posix
--SKIPIF--
<?php
if (!function_exists('posix_initgroups')) die('skip posix_initgroups() not found');
?>
--FILE--
<?php

var_dump(posix_initgroups('', 0));

?>
--EXPECT--
bool(false)
