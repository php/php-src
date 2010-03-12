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
/* Prototype  : string gzcompress(string data [, int level])
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
$extra_arg = 10;
var_dump( gzcompress($data, $level, $extra_arg) );

echo "\n-- Testing with incorrect compression level --\n";
$bad_level = 99;
var_dump(gzcompress($data, $bad_level));

class Tester {
    function Hello() {
        echo "Hello\n"; 
    }
}

echo "\n-- Testing with incorrect parameters --\n";
$testclass = new Tester();
var_dump(gzcompress($testclass));

var_dump(gzcompress($data, $testclass));


?>
===Done===
--EXPECTF--
*** Testing gzcompress() : error conditions ***

-- Testing gzcompress() function with Zero arguments --

Warning: gzcompress() expects at least 1 parameter, 0 given in %s on line %d
NULL

-- Testing gzcompress() function with more than expected no. of arguments --

Warning: gzcompress() expects at most 2 parameters, 3 given in %s on line %d
NULL

-- Testing with incorrect compression level --

Warning: gzcompress(): compression level (99) must be within -1..9 in %s on line %d
bool(false)

-- Testing with incorrect parameters --

Warning: gzcompress() expects parameter 1 to be string, object given in %s on line %d
NULL

Warning: gzcompress() expects parameter 2 to be long, object given in %s on line %d
NULL
===Done===