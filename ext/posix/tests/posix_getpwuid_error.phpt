--TEST--
Test posix_getpwuid() function : error conditions
--SKIPIF--
<?php
    if(!extension_loaded("posix")) print "skip - POSIX extension not loaded";
?>
--FILE--
<?php
echo "*** Testing posix_getpwuid() : error conditions ***\n";

echo "\n-- Testing posix_getpwuid() function negative uid --\n";
$uid = -99;
var_dump( posix_getpwuid($uid) );

echo "Done";
?>
--EXPECT--
*** Testing posix_getpwuid() : error conditions ***

-- Testing posix_getpwuid() function negative uid --
bool(false)
Done
