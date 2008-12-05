--TEST--
Test posix_getgrgid() function : error conditions 
--SKIPIF--
<?php 
	if(!extension_loaded("posix")) print "skip - POSIX extension not loaded"; 
?>
--FILE--
<?php
/* Prototype  : proto array posix_getgrgid(long gid)
 * Description: Group database access (POSIX.1, 9.2.1) 
 * Source code: ext/posix/posix.c
 * Alias to functions: 
 */

echo "*** Testing posix_getgrgid() : error conditions ***\n";

// Zero arguments
echo "\n-- Testing posix_getgrgid() function with Zero arguments --\n";
var_dump( posix_getgrgid() );

//Test posix_getgrgid with one more than the expected number of arguments
echo "\n-- Testing posix_getgrgid() function with more than expected no. of arguments --\n";

$extra_arg = 10;
$gid = 0;
var_dump( posix_getgrgid($gid, $extra_arg) );

echo "\n-- Testing posix_getgrgid() function with a negative group id --\n";
$gid = -999;
var_dump( posix_getgrgid($gid));

echo "Done";
?>
--EXPECTF--
*** Testing posix_getgrgid() : error conditions ***

-- Testing posix_getgrgid() function with Zero arguments --

Warning: posix_getgrgid() expects exactly 1 parameter, 0 given in %s on line %d
bool(false)

-- Testing posix_getgrgid() function with more than expected no. of arguments --

Warning: posix_getgrgid() expects exactly 1 parameter, 2 given in %s on line %d
bool(false)

-- Testing posix_getgrgid() function with a negative group id --
bool(false)
Done
