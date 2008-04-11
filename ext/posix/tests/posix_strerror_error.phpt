--TEST--
Test posix_strerror() function : error conditions 
--SKIPIF--
<?php 
	if(!extension_loaded("posix")) print "skip - POSIX extension not loaded"; 
?>
--FILE--
<?php
/* Prototype  : proto string posix_strerror(int errno)
 * Description: Retrieve the system error message associated with the given errno. 
 * Source code: ext/posix/posix.c
 * Alias to functions: 
 */

echo "*** Testing posix_strerror() : error conditions ***\n";

echo "\n-- Testing posix_strerror() function with Zero arguments --\n";
var_dump( posix_strerror() );

echo "\n-- Testing posix_strerror() function with more than expected no. of arguments --\n";
$errno = posix_get_last_error();
$extra_arg = 10;
var_dump( posix_strerror($errno, $extra_arg) );

echo "\n-- Testing posix_strerror() function with invalid error number --\n";
$errno = -999;
var_dump( posix_strerror($errno) );

echo "Done";
?>
--EXPECTF--
*** Testing posix_strerror() : error conditions ***

-- Testing posix_strerror() function with Zero arguments --

Warning: posix_strerror() expects exactly 1 parameter, 0 given in %s on line %d
bool(false)

-- Testing posix_strerror() function with more than expected no. of arguments --

Warning: posix_strerror() expects exactly 1 parameter, 2 given in %s on line %d
bool(false)

-- Testing posix_strerror() function with invalid error number --
string(%d) "Unknown error %d"
Done
