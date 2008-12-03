--TEST--
Test xml_get_current_column_number() function : error conditions 
--SKIPIF--
<?php 
if (!extension_loaded("xml")) {
	print "skip - XML extension not loaded"; 
}	 
?>
--FILE--
<?php
/* Prototype  : proto int xml_get_current_column_number(resource parser)
 * Description: Get current column number for an XML parser 
 * Source code: ext/xml/xml.c
 * Alias to functions: 
 */

/*
 * add a comment here to say what the test is supposed to do
 */

echo "*** Testing xml_get_current_column_number() : error conditions ***\n";

// Zero arguments
echo "\n-- Testing xml_get_current_column_number() function with Zero arguments --\n";
var_dump( xml_get_current_column_number() );

//Test xml_get_current_column_number with one more than the expected number of arguments
echo "\n-- Testing xml_get_current_column_number() function with more than expected no. of arguments --\n";

$extra_arg = 10;
var_dump( xml_get_current_column_number(null, $extra_arg) );

echo "Done";
?>
--EXPECTF--
*** Testing xml_get_current_column_number() : error conditions ***

-- Testing xml_get_current_column_number() function with Zero arguments --

Warning: Wrong parameter count for xml_get_current_column_number() in %s on line %d
NULL

-- Testing xml_get_current_column_number() function with more than expected no. of arguments --

Warning: Wrong parameter count for xml_get_current_column_number() in %s on line %d
NULL
Done
