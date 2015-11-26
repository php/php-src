--TEST--
Test xml_set_external_entity_ref_handler() function : error conditions 
--SKIPIF--
<?php 
if (!extension_loaded("xml")) {
	print "skip - XML extension not loaded"; 
}	 
?>
--FILE--
<?php
/* Prototype  : proto int xml_set_external_entity_ref_handler(resource parser, string hdl)
 * Description: Set up external entity reference handler 
 * Source code: ext/xml/xml.c
 * Alias to functions: 
 */

echo "*** Testing xml_set_external_entity_ref_handler() : error conditions ***\n";


//Test xml_set_external_entity_ref_handler with one more than the expected number of arguments
echo "\n-- Testing xml_set_external_entity_ref_handler() function with more than expected no. of arguments --\n";

$hdl = 'string_val';
$extra_arg = 10;
var_dump( xml_set_external_entity_ref_handler(null, $hdl, $extra_arg) );

// Testing xml_set_external_entity_ref_handler with one less than the expected number of arguments
echo "\n-- Testing xml_set_external_entity_ref_handler() function with less than expected no. of arguments --\n";

var_dump( xml_set_external_entity_ref_handler(null) );

echo "Done";
?>
--EXPECTF--
*** Testing xml_set_external_entity_ref_handler() : error conditions ***

-- Testing xml_set_external_entity_ref_handler() function with more than expected no. of arguments --

Warning: xml_set_external_entity_ref_handler() expects exactly 2 parameters, 3 given in %s on line %d
NULL

-- Testing xml_set_external_entity_ref_handler() function with less than expected no. of arguments --

Warning: xml_set_external_entity_ref_handler() expects exactly 2 parameters, 1 given in %s on line %d
NULL
Done

