--TEST--
Test posix_getpgid() function : error conditions
--SKIPIF--
<?php
if((!extension_loaded("posix")) || (!function_exists("posix_getpgid"))) {
     print "skip - POSIX extension not loaded or posix_getpgid() does not exist";
}
?>
--FILE--
<?php
echo "*** Testing posix_getpgid() : error conditions ***\n";

echo "\n-- Testing posix_getpgid() with negative pid  --\n";
$pid = -99;
var_dump( posix_getpgid($pid) );

echo "Done";
?>
--EXPECT--
*** Testing posix_getpgid() : error conditions ***

-- Testing posix_getpgid() with negative pid  --
bool(false)
Done
