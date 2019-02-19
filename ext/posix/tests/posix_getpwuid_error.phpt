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

echo "\n-- Testing posix_getpwuid() function negative uid --\n";
$uid = -99;
var_dump( posix_getpwuid($uid) );

echo "Done";
?>
--EXPECTF--
*** Testing posix_getpwuid() : error conditions ***

-- Testing posix_getpwuid() function negative uid --
bool(false)
Done
