--TEST--
Test xml_parser_get_option() function : error conditions
--SKIPIF--
<?php
if (!extension_loaded("xml")) {
	print "skip - XML extension not loaded";
}
?>
--FILE--
<?php
/* Prototype  : proto int xml_parser_get_option(resource parser, int option)
 * Description: Get options from an XML parser
 * Source code: ext/xml/xml.c
 * Alias to functions:
 */

echo "*** Testing xml_parser_get_option() : error conditions ***\n";


//Test xml_parser_get_option with one more than the expected number of arguments
echo "\n-- Testing xml_parser_get_option() function with more than expected no. of arguments --\n";

$option = 10;
$extra_arg = 10;
var_dump( xml_parser_get_option(null, $option, $extra_arg) );

// Testing xml_parser_get_option with one less than the expected number of arguments
echo "\n-- Testing xml_parser_get_option() function with less than expected no. of arguments --\n";

var_dump( xml_parser_get_option(null) );
echo "Done";
?>
--EXPECTF--
*** Testing xml_parser_get_option() : error conditions ***

-- Testing xml_parser_get_option() function with more than expected no. of arguments --

Warning: xml_parser_get_option() expects exactly 2 parameters, 3 given in %s on line %d
NULL

-- Testing xml_parser_get_option() function with less than expected no. of arguments --

Warning: xml_parser_get_option() expects exactly 2 parameters, 1 given in %s on line %d
NULL
Done
