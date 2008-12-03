--TEST--
Test utf8_decode() function : error conditions 
--SKIPIF--
<?php 
if (!extension_loaded("xml")) {
	print "skip - XML extension not loaded"; 
}	 
?>
--FILE--
<?php
/* Prototype  : proto string utf8_decode(string data)
 * Description: Converts a UTF-8 encoded string to ISO-8859-1 
 * Source code: ext/xml/xml.c
 * Alias to functions: 
 */

/*
 * add a comment here to say what the test is supposed to do
 */

echo "*** Testing utf8_decode() : error conditions ***\n";

// Zero arguments
echo "\n-- Testing utf8_decode() function with Zero arguments --\n";
var_dump( utf8_decode() );

//Test utf8_decode with one more than the expected number of arguments
echo "\n-- Testing utf8_decode() function with more than expected no. of arguments --\n";
$data = 'string_val';
$extra_arg = 10;
var_dump( utf8_decode($data, $extra_arg) );

echo "Done";
?>
--EXPECTF--
*** Testing utf8_decode() : error conditions ***

-- Testing utf8_decode() function with Zero arguments --

Warning: Wrong parameter count for utf8_decode() in %s on line %d
NULL

-- Testing utf8_decode() function with more than expected no. of arguments --

Warning: Wrong parameter count for utf8_decode() in %s on line %d
NULL
Done