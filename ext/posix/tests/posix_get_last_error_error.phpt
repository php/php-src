--TEST--
Test posix_get_last_error() function : error conditions
--SKIPIF--
<?php
	if(!extension_loaded("posix")) print "skip - POSIX extension not loaded";
?>
--FILE--
<?php
/* Prototype  : proto int posix_get_last_error(void)
 * Description: Retrieve the error number set by the last posix function which failed.
 * Source code: ext/posix/posix.c
 * Alias to functions: posix_errno
 */

echo "*** Testing posix_get_last_error() : error conditions ***\n";

// One argument
echo "\n-- Testing posix_get_last_error() function with one argument --\n";
$extra_arg = 10;
var_dump( posix_get_last_error($extra_arg) );

echo "Done";
?>
--EXPECTF--
*** Testing posix_get_last_error() : error conditions ***

-- Testing posix_get_last_error() function with one argument --

Warning: posix_get_last_error() expects exactly 0 parameters, 1 given in %s on line %d
NULL
Done
