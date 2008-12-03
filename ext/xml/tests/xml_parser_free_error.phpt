--TEST--
Test xml_parser_free() function : error conditions 
--SKIPIF--
<?php 
if (!extension_loaded("xml")) {
	print "skip - XML extension not loaded"; 
}	 
?>
--FILE--
<?php
/* Prototype  : proto int xml_parser_free(resource parser)
 * Description: Free an XML parser 
 * Source code: ext/xml/xml.c
 * Alias to functions: 
 */

echo "*** Testing xml_parser_free() : error conditions ***\n";

// Zero arguments
echo "\n-- Testing xml_parser_free() function with Zero arguments --\n";
var_dump( xml_parser_free() );

//Test xml_parser_free with one more than the expected number of arguments
echo "\n-- Testing xml_parser_free() function with more than expected no. of arguments --\n";

$extra_arg = 10;
var_dump( xml_parser_free(null, $extra_arg) );

echo "Done";
?>
--EXPECTF--
*** Testing xml_parser_free() : error conditions ***

-- Testing xml_parser_free() function with Zero arguments --

Warning: Wrong parameter count for xml_parser_free() in %s on line %d
NULL

-- Testing xml_parser_free() function with more than expected no. of arguments --

Warning: Wrong parameter count for xml_parser_free() in %s on line %d
NULL
Done
