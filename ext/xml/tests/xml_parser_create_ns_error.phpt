--TEST--
Test xml_parser_create_ns() function : error conditions
--SKIPIF--
<?php
if (!extension_loaded("xml")) {
	print "skip - XML extension not loaded";
}
?>
--FILE--
<?php
/* Prototype  : proto resource xml_parser_create_ns([string encoding [, string sep]])
 * Description: Create an XML parser
 * Source code: ext/xml/xml.c
 * Alias to functions:
 */

echo "*** Testing xml_parser_create_ns() : error conditions ***\n";


//Test xml_parser_create_ns with one more than the expected number of arguments
echo "\n-- Testing xml_parser_create_ns() function with more than expected no. of arguments --\n";
$encoding = 'string_val';
$sep = 'string_val';
$extra_arg = 10;
var_dump( xml_parser_create_ns($encoding, $sep, $extra_arg) );

echo "Done";
?>
--EXPECTF--
*** Testing xml_parser_create_ns() : error conditions ***

-- Testing xml_parser_create_ns() function with more than expected no. of arguments --

Warning: xml_parser_create_ns() expects at most 2 parameters, 3 given in %s on line %d
bool(false)
Done
