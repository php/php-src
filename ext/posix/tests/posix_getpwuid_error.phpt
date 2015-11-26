--TEST--
Test posix_getpwuid() function : error conditions 
--SKIPIF--
<?php 
	if(!extension_loaded("posix")) print "skip - POSIX extension not loaded"; 
?>
--FILE--
<?php
/* Prototype  : proto array posix_getpwuid(long uid)
 * Description: User database access (POSIX.1, 9.2.2) 
 * Source code: ext/posix/posix.c
 * Alias to functions: 
 */

echo "*** Testing posix_getpwuid() : error conditions ***\n";

echo "\n-- Testing posix_getpwuid() function with Zero arguments --\n";
var_dump( posix_getpwuid() );

echo "\n-- Testing posix_getpwuid() function with more than expected no. of arguments --\n";
$uid = posix_getuid();
$extra_arg = 10;
var_dump( posix_getpwuid($uid, $extra_arg) );

echo "\n-- Testing posix_getpwuid() function negative uid --\n";
$uid = -99;
var_dump( posix_getpwuid($uid) );

echo "Done";
?>
--EXPECTF--
*** Testing posix_getpwuid() : error conditions ***

-- Testing posix_getpwuid() function with Zero arguments --

Warning: posix_getpwuid() expects exactly 1 parameter, 0 given in %s on line %d
bool(false)

-- Testing posix_getpwuid() function with more than expected no. of arguments --

Warning: posix_getpwuid() expects exactly 1 parameter, 2 given in %s on line %d
bool(false)

-- Testing posix_getpwuid() function negative uid --
bool(false)
Done
