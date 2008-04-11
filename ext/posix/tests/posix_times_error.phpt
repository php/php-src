--TEST--
Test posix_times() function : error conditions 
--SKIPIF--
<?php 
	if(!extension_loaded("posix")) print "skip - POSIX extension not loaded"; 
?>
--FILE--
<?php
/* Prototype  : proto array posix_times(void)
 * Description: Get process times (POSIX.1, 4.5.2) 
 * Source code: ext/posix/posix.c
 * Alias to functions: 
 */

echo "*** Testing posix_times() : error conditions ***\n";

// One argument
echo "\n-- Testing posix_times() function with one argument --\n";
$extra_arg = 10;;
var_dump( posix_times($extra_arg) );

echo "Done";
?>
--EXPECTF--
*** Testing posix_times() : error conditions ***

-- Testing posix_times() function with one argument --

Warning: Wrong parameter count for posix_times() in %s on line %d
NULL
Done
