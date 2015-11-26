--TEST--
Test xml_error_string() function : error conditions 
--SKIPIF--
<?php 
if (!extension_loaded("xml")) {
	print "skip - XML extension not loaded"; 
}	 
?>
--FILE--
<?php
/* Prototype  : proto string xml_error_string(int code)
 * Description: Get XML parser error string 
 * Source code: ext/xml/xml.c
 * Alias to functions: 
 */

echo "*** Testing xml_error_string() : error conditions ***\n";

// Zero arguments
echo "\n-- Testing xml_error_string() function with Zero arguments --\n";
var_dump( xml_error_string() );

//Test xml_error_string with one more than the expected number of arguments
echo "\n-- Testing xml_error_string() function with more than expected no. of arguments --\n";
$code = 10;
$extra_arg = 10;
var_dump( xml_error_string($code, $extra_arg) );

echo "Done";
?>
--EXPECTF--
*** Testing xml_error_string() : error conditions ***

-- Testing xml_error_string() function with Zero arguments --

Warning: xml_error_string() expects exactly 1 parameter, 0 given in %s on line %d
NULL

-- Testing xml_error_string() function with more than expected no. of arguments --

Warning: xml_error_string() expects exactly 1 parameter, 2 given in %s on line %d
NULL
Done

