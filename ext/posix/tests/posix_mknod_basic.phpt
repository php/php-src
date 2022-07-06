--TEST--
posix_mknod(): Basic tests
--SKIPIF--
<?php
if (!extension_loaded('posix')) die('skip - POSIX extension not loaded');
if (!function_exists('posix_mknod')) die('skip posix_mknod() not found');
?>
--FILE--
<?php
echo "Basic test of POSIX posix_mknod function\n";
var_dump(posix_mknod(NULL, NULL, NULL, NULL));

?>
===DONE====
--EXPECT--
Basic test of POSIX posix_mknod function
bool(false)
===DONE====
