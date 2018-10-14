--TEST--
Test gzopen() function : error conditions
--SKIPIF--
<?php
if (!extension_loaded("zlib")) {
	print "skip - ZLIB extension not loaded";
}
?>
--FILE--
<?php
/* Prototype  : resource gzopen(string filename, string mode [, int use_include_path])
 * Description: Open a .gz-file and return a .gz-file pointer
 * Source code: ext/zlib/zlib.c
 * Alias to functions:
 */

echo "*** Testing gzopen() : error conditions ***\n";


//Test gzopen with one more than the expected number of arguments
echo "\n-- Testing gzopen() function with more than expected no. of arguments --\n";
$filename = 'string_val';
$mode = 'string_val';
$use_include_path = 10;
$extra_arg = 10;
var_dump( gzopen($filename, $mode, $use_include_path, $extra_arg) );

// Testing gzopen with one less than the expected number of arguments
echo "\n-- Testing gzopen() function with less than expected no. of arguments --\n";
$filename = 'string_val';
var_dump( gzopen($filename) );

?>
===DONE===
--EXPECTF--
*** Testing gzopen() : error conditions ***

-- Testing gzopen() function with more than expected no. of arguments --

Warning: gzopen() expects at most 3 parameters, 4 given in %s on line %d
NULL

-- Testing gzopen() function with less than expected no. of arguments --

Warning: gzopen() expects at least 2 parameters, 1 given in %s on line %d
NULL
===DONE===
