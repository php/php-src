--TEST--
Test xml_get_error_code() function : error conditions 
--SKIPIF--
<?php 
if (!extension_loaded("xml")) {
	print "skip - XML extension not loaded"; 
}	 
?>
--FILE--
<?php
/* Prototype  : proto int xml_get_error_code(resource parser)
 * Description: Get XML parser error code 
 * Source code: ext/xml/xml.c
 * Alias to functions: 
 */

echo "*** Testing xml_get_error_code() : error conditions ***\n";

// Zero arguments
echo "\n-- Testing xml_get_error_code() function with Zero arguments --\n";
var_dump( xml_get_error_code() );

//Test xml_get_error_code with one more than the expected number of arguments
echo "\n-- Testing xml_get_error_code() function with more than expected no. of arguments --\n";

$extra_arg = 10;
var_dump( xml_get_error_code(null, $extra_arg) );

echo "Done";
?>
--EXPECTF--
*** Testing xml_get_error_code() : error conditions ***

-- Testing xml_get_error_code() function with Zero arguments --

Warning: Wrong parameter count for xml_get_error_code() in %s on line %d
NULL

-- Testing xml_get_error_code() function with more than expected no. of arguments --

Warning: Wrong parameter count for xml_get_error_code() in %s on line %d
NULL
Done
