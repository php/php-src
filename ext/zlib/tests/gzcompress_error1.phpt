--TEST--
Test gzcompress() function : error conditions 
--SKIPIF--
<?php 
if (!extension_loaded("zlib")) {
	print "skip - ZLIB extension not loaded"; 
}	 
?>
--FILE--
<?php
/* Prototype  : string gzcompress(string data [, int level, [int encoding]])
 * Description: Gzip-compress a string 
 * Source code: ext/zlib/zlib.c
 * Alias to functions: 
 */

/*
 * add a comment here to say what the test is supposed to do
 */

echo "*** Testing gzcompress() : error conditions ***\n";

// Zero arguments
echo "\n-- Testing gzcompress() function with Zero arguments --\n";
var_dump( gzcompress() );

//Test gzcompress with one more than the expected number of arguments
echo "\n-- Testing gzcompress() function with more than expected no. of arguments --\n";
$data = 'string_val';
$level = 2;
$encoding = ZLIB_ENCODING_RAW;
$extra_arg = 10;
var_dump( gzcompress($data, $level, $encoding, $extra_arg) );

echo "\n-- Testing with incorrect compression level --\n";
$bad_level = 99;
var_dump(gzcompress($data, $bad_level));

echo "\n-- Testing with invalid encoding --\n";
$data = 'string_val';
$encoding = 99;
var_dump(gzcompress($data, $level, $encoding));

echo "\n-- Testing with incorrect parameters --\n";

class Tester {
    function Hello() {
        echo "Hello\n"; 
    }
}

$testclass = new Tester();
var_dump(gzcompress($testclass));

?>
===Done===
--EXPECTF--
*** Testing gzcompress() : error conditions ***

-- Testing gzcompress() function with Zero arguments --

Warning: gzcompress() expects at least 1 parameter, 0 given in %s on line %d
NULL

-- Testing gzcompress() function with more than expected no. of arguments --

Warning: gzcompress() expects at most 3 parameters, 4 given in %s on line %d
NULL

-- Testing with incorrect compression level --

Warning: gzcompress(): compression level (99) must be within -1..9 in %s on line %d
bool(false)

-- Testing with invalid encoding --

Warning: gzcompress(): encoding mode must be either ZLIB_ENCODING_RAW, ZLIB_ENCODING_GZIP or ZLIB_ENCODING_DEFLATE in %s on line %d
bool(false)

-- Testing with incorrect parameters --

Warning: gzcompress() expects parameter 1 to be string, object given in %s on line %d
NULL
===Done===
