--TEST--
Test xml_get_current_line_number() function : usage variations
--SKIPIF--
<?php
if (!extension_loaded("xml")) {
	print "skip - XML extension not loaded";
}
?>
--FILE--
<?php
/* Prototype  : proto int xml_get_current_line_number(resource parser)
 * Description: Get current line number for an XML parser
 * Source code: ext/xml/xml.c
 * Alias to functions:
 */

echo "*** Testing xml_get_current_line_number() : usage variations ***\n";
error_reporting(E_ALL & ~E_NOTICE);

class aClass {
   function __toString() {
       return "Some Ascii Data";
   }
}
// Initialise function arguments not being substituted (if any)

//get an unset variable
$unset_var = 10;
unset ($unset_var);

$fp = fopen(__FILE__, "r");

//array of values to iterate over
$values = array(

      // int data
      0,
      1,
      12345,
      -2345,

      // float data
      10.5,
      -10.5,
      10.1234567e10,
      10.7654321E-10,
      .5,

      // array data
      array(),
      array(0),
      array(1),
      array(1, 2),
      array('color' => 'red', 'item' => 'pen'),

      // null data
      NULL,
      null,

      // boolean data
      true,
      false,
      TRUE,
      FALSE,

      // empty data
      "",
      '',

      // string data
      "string",
      'string',

      // object data
      new aClass(),

      // resource data
      $fp,

      // undefined data
      $undefined_var,

      // unset data
      $unset_var,
);

// loop through each element of the array for parser

foreach($values as $value) {
      echo @"\nArg value $value \n";
      var_dump( xml_get_current_line_number($value) );
};

fclose($fp);
echo "Done";
?>
--EXPECTF--
*** Testing xml_get_current_line_number() : usage variations ***

Arg value 0 

Warning: xml_get_current_line_number() expects parameter 1 to be resource, integer given in %s on line %d
NULL

Arg value 1 

Warning: xml_get_current_line_number() expects parameter 1 to be resource, integer given in %s on line %d
NULL

Arg value 12345 

Warning: xml_get_current_line_number() expects parameter 1 to be resource, integer given in %s on line %d
NULL

Arg value -2345 

Warning: xml_get_current_line_number() expects parameter 1 to be resource, integer given in %s on line %d
NULL

Arg value 10.5 

Warning: xml_get_current_line_number() expects parameter 1 to be resource, float given in %s on line %d
NULL

Arg value -10.5 

Warning: xml_get_current_line_number() expects parameter 1 to be resource, float given in %s on line %d
NULL

Arg value 101234567000 

Warning: xml_get_current_line_number() expects parameter 1 to be resource, float given in %s on line %d
NULL

Arg value 1.07654321E-9 

Warning: xml_get_current_line_number() expects parameter 1 to be resource, float given in %s on line %d
NULL

Arg value 0.5 

Warning: xml_get_current_line_number() expects parameter 1 to be resource, float given in %s on line %d
NULL

Arg value Array 

Warning: xml_get_current_line_number() expects parameter 1 to be resource, array given in %s on line %d
NULL

Arg value Array 

Warning: xml_get_current_line_number() expects parameter 1 to be resource, array given in %s on line %d
NULL

Arg value Array 

Warning: xml_get_current_line_number() expects parameter 1 to be resource, array given in %s on line %d
NULL

Arg value Array 

Warning: xml_get_current_line_number() expects parameter 1 to be resource, array given in %s on line %d
NULL

Arg value Array 

Warning: xml_get_current_line_number() expects parameter 1 to be resource, array given in %s on line %d
NULL

Arg value  

Warning: xml_get_current_line_number() expects parameter 1 to be resource, null given in %s on line %d
NULL

Arg value  

Warning: xml_get_current_line_number() expects parameter 1 to be resource, null given in %s on line %d
NULL

Arg value 1 

Warning: xml_get_current_line_number() expects parameter 1 to be resource, boolean given in %s on line %d
NULL

Arg value  

Warning: xml_get_current_line_number() expects parameter 1 to be resource, boolean given in %s on line %d
NULL

Arg value 1 

Warning: xml_get_current_line_number() expects parameter 1 to be resource, boolean given in %s on line %d
NULL

Arg value  

Warning: xml_get_current_line_number() expects parameter 1 to be resource, boolean given in %s on line %d
NULL

Arg value  

Warning: xml_get_current_line_number() expects parameter 1 to be resource, string given in %s on line %d
NULL

Arg value  

Warning: xml_get_current_line_number() expects parameter 1 to be resource, string given in %s on line %d
NULL

Arg value string 

Warning: xml_get_current_line_number() expects parameter 1 to be resource, string given in %s on line %d
NULL

Arg value string 

Warning: xml_get_current_line_number() expects parameter 1 to be resource, string given in %s on line %d
NULL

Arg value Some Ascii Data 

Warning: xml_get_current_line_number() expects parameter 1 to be resource, object given in %s on line %d
NULL

Arg value Resource id %s

Warning: xml_get_current_line_number(): supplied resource is not a valid XML Parser resource in %s on line %d
bool(false)

Arg value  

Warning: xml_get_current_line_number() expects parameter 1 to be resource, null given in %s on line %d
NULL

Arg value  

Warning: xml_get_current_line_number() expects parameter 1 to be resource, null given in %s on line %d
NULL
Done
