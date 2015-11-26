--TEST--
Test posix_getpid() function : error conditions 
--SKIPIF--
<?php 
	if(!extension_loaded("posix")) print "skip - POSIX extension not loaded"; 
?>
--FILE--
<?php
/* Prototype  : proto int posix_getpid(void)
 * Description: Get the current process id (POSIX.1, 4.1.1) 
 * Source code: ext/posix/posix.c
 * Alias to functions: 
 */

echo "*** Testing posix_getpid() : error conditions ***\n";

// One argument
echo "\n-- Testing posix_getpid() function with one argument --\n";
$extra_arg = 10;
var_dump( posix_getpid($extra_arg) );

echo "Done";
?>
--EXPECTF--
*** Testing posix_getpid() : error conditions ***

-- Testing posix_getpid() function with one argument --

Warning: posix_getpid() expects exactly 0 parameters, 1 given in %s on line %d
NULL
Done
