--TEST--
Test xml_parser_create() function : error conditions
--SKIPIF--
<?php
if (!extension_loaded("xml")) {
	print "skip - XML extension not loaded";
}
?>
--FILE--
<?php
/* Prototype  : proto resource xml_parser_create([string encoding])
 * Description: Create an XML parser
 * Source code: ext/xml/xml.c
 * Alias to functions:
 */

echo "*** Testing xml_parser_create() : error conditions ***\n";


//Test xml_parser_create with one more than the expected number of arguments
echo "\n-- Testing xml_parser_create() function with more than expected no. of arguments --\n";
$encoding = 'utf-8';
$extra_arg = 10;
var_dump( xml_parser_create($encoding, $extra_arg) );

echo "Done";
?>
--EXPECTF--
*** Testing xml_parser_create() : error conditions ***

-- Testing xml_parser_create() function with more than expected no. of arguments --

Warning: xml_parser_create() expects at most 1 parameter, 2 given in %s on line %d
bool(false)
Done
