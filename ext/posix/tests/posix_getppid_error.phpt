--TEST--
Test posix_getppid() function : error conditions 
--SKIPIF--
<?php 
	if(!extension_loaded("posix")) print "skip - POSIX extension not loaded"; 
?>
--FILE--
<?php
/* Prototype  : proto int posix_getppid(void)
 * Description: Get the parent process id (POSIX.1, 4.1.1) 
 * Source code: ext/posix/posix.c
 * Alias to functions: 
 */

echo "*** Testing posix_getppid() : error conditions ***\n";

// One argument
echo "\n-- Testing posix_getppid() function with one argument --\n";
$extra_arg = 10;
var_dump( posix_getppid($extra_arg) );

echo "Done";
?>
--EXPECTF--
*** Testing posix_getppid() : error conditions ***

-- Testing posix_getppid() function with one argument --

Warning: Wrong parameter count for posix_getppid() in %s on line %d
NULL
Done
