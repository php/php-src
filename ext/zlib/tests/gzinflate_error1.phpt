--TEST--
Test gzinflate() function : error conditions 
--SKIPIF--
<?php 
if (!extension_loaded("zlib")) {
	print "skip - ZLIB extension not loaded"; 
}	 
?>
--FILE--
<?php
/* Prototype  : string gzinflate(string data [, int length])
 * Description: Unzip a gzip-compressed string 
 * Source code: ext/zlib/zlib.c
 * Alias to functions: 
 */

include(dirname(__FILE__) . '/data.inc');

echo "*** Testing gzinflate() : error conditions ***\n";

echo "\n-- Testing gzcompress() function with Zero arguments --\n";
var_dump( gzinflate() );

echo "\n-- Testing gzcompress() function with more than expected no. of arguments --\n";
$data = 'string_val';
$length = 10;
$extra_arg = 10;
var_dump( gzinflate($data, $length, $extra_arg) );

echo "\n-- Testing with a buffer that is too small --\n";
$short_len = strlen($data) - 1;
$compressed = gzcompress($data);

var_dump(gzinflate($compressed, $short_len));

echo "\n-- Testing with incorrect parameters --\n";

class Tester {
    function Hello() {
        echo "Hello\n"; 
    }
}

$testclass = new Tester();
var_dump(gzinflate($testclass));
var_dump(gzinflate($data, $testclass));

?>
===DONE===
--EXPECTF--
*** Testing gzinflate() : error conditions ***

-- Testing gzcompress() function with Zero arguments --

Warning: gzinflate() expects at least 1 parameter, 0 given in %s on line %d
NULL

-- Testing gzcompress() function with more than expected no. of arguments --

Warning: gzinflate() expects at most 2 parameters, 3 given in %s on line %d
NULL

-- Testing with a buffer that is too small --

Warning: gzinflate(): data error in %s on line %d
bool(false)

-- Testing with incorrect parameters --

Warning: gzinflate() expects parameter 1 to be string, object given in %s on line %d
NULL

Warning: gzinflate() expects parameter 2 to be integer, object given in %s on line %d
NULL
===DONE===