--TEST--
Test posix_getgrgid() function : error conditions
--SKIPIF--
<?php
    if(!extension_loaded("posix")) print "skip - POSIX extension not loaded";
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
