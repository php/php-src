--TEST--
Test posix_getuid() function : error conditions
--SKIPIF--
<?php
	if(!extension_loaded("posix")) print "skip - POSIX extension not loaded";
?>
--FILE--
<?php
/* Prototype  : proto int posix_getuid(void)
 * Description: Get the current user id (POSIX.1, 4.2.1)
 * Source code: ext/posix/posix.c
 * Alias to functions:
 */

echo "*** Testing posix_getuid() : error conditions ***\n";

// One argument
echo "\n-- Testing posix_getuid() function with one argument --\n";
$extra_arg = 10;
var_dump( posix_getuid($extra_arg) );

echo "Done";
?>
--EXPECTF--
*** Testing posix_getuid() : error conditions ***

-- Testing posix_getuid() function with one argument --

Warning: posix_getuid() expects exactly 0 parameters, 1 given in %s on line %d
NULL
Done
