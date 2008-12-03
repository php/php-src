--TEST--
Test xml_set_object() function : error conditions 
--SKIPIF--
<?php 
if (!extension_loaded("xml")) {
	print "skip - XML extension not loaded"; 
}	 
?>
--FILE--
<?php
/* Prototype  : proto int xml_set_object(resource parser, object &obj)
 * Description: Set up object which should be used for callbacks 
 * Source code: ext/xml/xml.c
 * Alias to functions: 
 */

echo "*** Testing xml_set_object() : error conditions ***\n";


//Test xml_set_object with one more than the expected number of arguments
echo "\n-- Testing xml_set_object() function with more than expected no. of arguments --\n";

//WARNING: Unable to initialise parser of type resource

$obj = new stdclass();
$extra_arg = 10;
var_dump( xml_set_object(null, $obj, $extra_arg) );

// Testing xml_set_object with one less than the expected number of arguments
echo "\n-- Testing xml_set_object() function with less than expected no. of arguments --\n";

//WARNING: Unable to initialise parser of type resource

var_dump( xml_set_object(null) );

echo "Done";
?>
--EXPECTF--
*** Testing xml_set_object() : error conditions ***

-- Testing xml_set_object() function with more than expected no. of arguments --

Warning: xml_set_object() expects exactly 2 parameters, 3 given in %s on line %d
NULL

-- Testing xml_set_object() function with less than expected no. of arguments --

Warning: xml_set_object() expects exactly 2 parameters, 1 given in %s on line %d
NULL
Done

