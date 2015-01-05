--TEST--
Test posix_uname() function : error conditions 
--SKIPIF--
<?php 
	if(!extension_loaded("posix")) print "skip - POSIX extension not loaded"; 
?>
--FILE--
<?php
/* Prototype  : proto array posix_uname(void)
 * Description: Get system name (POSIX.1, 4.4.1) 
 * Source code: ext/posix/posix.c
 * Alias to functions: 
 */

echo "*** Testing posix_uname() : error conditions ***\n";

// One argument
echo "\n-- Testing posix_uname() function with one argument --\n";
$extra_arg = 10;;
var_dump( posix_uname($extra_arg) );

echo "Done";
?>
--EXPECTF--
*** Testing posix_uname() : error conditions ***

-- Testing posix_uname() function with one argument --

Warning: posix_uname() expects exactly 0 parameters, 1 given in %s on line %d
NULL
Done
