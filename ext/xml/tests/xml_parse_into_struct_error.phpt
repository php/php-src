--TEST--
Test xml_parse_into_struct() function : error conditions 
--SKIPIF--
<?php 
if (!extension_loaded("xml")) {
	print "skip - XML extension not loaded"; 
}	 
?>
--FILE--
<?php
/* Prototype  : proto int xml_parse_into_struct(resource parser, string data, array &struct, array &index)
 * Description: Parsing a XML document 
 * Source code: ext/xml/xml.c
 * Alias to functions: 
 */

echo "*** Testing xml_parse_into_struct() : error conditions ***\n";

//Test xml_parse_into_struct with one more than the expected number of arguments
echo "\n-- Testing xml_parse_into_struct() function with more than expected no. of arguments --\n";

$data = 'string_val';
$struct = array(1, 2);
$index = array(1, 2);
$extra_arg = 10;
var_dump( xml_parse_into_struct(null, $data, $struct, $index, $extra_arg) );

// Testing xml_parse_into_struct with one less than the expected number of arguments
echo "\n-- Testing xml_parse_into_struct() function with less than expected no. of arguments --\n";

$data = 'string_val';
var_dump( xml_parse_into_struct(null, $data) );

echo "Done";
?>
--EXPECTF--
*** Testing xml_parse_into_struct() : error conditions ***

-- Testing xml_parse_into_struct() function with more than expected no. of arguments --

Warning: xml_parse_into_struct() expects at most 4 parameters, 5 given in %s on line %d
NULL

-- Testing xml_parse_into_struct() function with less than expected no. of arguments --

Warning: xml_parse_into_struct() expects at least 3 parameters, 2 given in %s on line %d
NULL
Done

