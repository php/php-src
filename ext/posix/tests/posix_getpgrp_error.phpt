--TEST--
Test posix_getpgrp() function : error conditions 
--SKIPIF--
<?php 
	if(!extension_loaded("posix")) print "skip - POSIX extension not loaded"; 
?>
--FILE--
<?php
/* Prototype  : proto int posix_getpgrp(void)
 * Description: Get current process group id (POSIX.1, 4.3.1) 
 * Source code: ext/posix/posix.c
 * Alias to functions: 
 */

echo "*** Testing posix_getpgrp() : error conditions ***\n";

// One argument
echo "\n-- Testing posix_getpgrp() function with one argument --\n";
$extra_arg = 10;
var_dump( posix_getpgrp($extra_arg) );

echo "Done";
?>
--EXPECTF--
*** Testing posix_getpgrp() : error conditions ***

-- Testing posix_getpgrp() function with one argument --

Warning: posix_getpgrp() expects exactly 0 parameters, 1 given in %s on line %d
NULL
Done
