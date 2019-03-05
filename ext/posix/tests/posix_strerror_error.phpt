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

echo "\n-- Testing posix_strerror() function with invalid error number --\n";
$errno = -999;
echo gettype( posix_strerror($errno) )."\n";

echo "Done";
?>
--EXPECTF--
*** Testing posix_strerror() : error conditions ***

-- Testing posix_strerror() function with invalid error number --
string
Done
