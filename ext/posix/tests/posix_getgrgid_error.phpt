--TEST--
Test posix_getgrgid() function : error conditions
--SKIPIF--
<?php
if(!extension_loaded("posix")) die("skip - POSIX extension not loaded");
if (getenv('SKIP_ASAN')) die('skip LSan crashes when firebird is loaded');
?>
--FILE--
<?php
echo "*** Testing posix_getgrgid() : error conditions ***\n";

echo "\n-- Testing posix_getgrgid() function with a negative group id --\n";
$gid = -999;
var_dump( posix_getgrgid($gid));

echo "Done";
?>
--EXPECT--
*** Testing posix_getgrgid() : error conditions ***

-- Testing posix_getgrgid() function with a negative group id --
bool(false)
Done
