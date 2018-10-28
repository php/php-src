--TEST--
Test xml_set_object() function : usage variations
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

echo "*** Testing xml_set_object() : usage variations ***\n";
error_reporting(E_ALL & ~E_NOTICE);
// Initialise function arguments not being substituted (if any)

$parser = xml_parser_create();
$fp = fopen(__FILE__, "r");


//get an unset variable
$unset_var = 10;
unset ($unset_var);

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

      // resource data
      $fp,

      // undefined data
      $undefined_var,

      // unset data
      $unset_var,
);

// loop through each element of the array for obj

foreach($values as $value) {
      echo @"\nArg value $value \n";
      var_dump( xml_set_object($parser, $value) );
};

xml_parser_free($parser);
fclose($fp);

echo "Done";
?>
--EXPECTF--
*** Testing xml_set_object() : usage variations ***

Arg value 0 

Warning: xml_set_object() expects parameter 2 to be object, int given in %s on line %d
NULL

Arg value 1 

Warning: xml_set_object() expects parameter 2 to be object, int given in %s on line %d
NULL

Arg value 12345 

Warning: xml_set_object() expects parameter 2 to be object, int given in %s on line %d
NULL

Arg value -2345 

Warning: xml_set_object() expects parameter 2 to be object, int given in %s on line %d
NULL

Arg value 10.5 

Warning: xml_set_object() expects parameter 2 to be object, float given in %s on line %d
NULL

Arg value -10.5 

Warning: xml_set_object() expects parameter 2 to be object, float given in %s on line %d
NULL

Arg value 101234567000 

Warning: xml_set_object() expects parameter 2 to be object, float given in %s on line %d
NULL

Arg value 1.07654321E-9 

Warning: xml_set_object() expects parameter 2 to be object, float given in %s on line %d
NULL

Arg value 0.5 

Warning: xml_set_object() expects parameter 2 to be object, float given in %s on line %d
NULL

Arg value Array 

Warning: xml_set_object() expects parameter 2 to be object, array given in %s on line %d
NULL

Arg value Array 

Warning: xml_set_object() expects parameter 2 to be object, array given in %s on line %d
NULL

Arg value Array 

Warning: xml_set_object() expects parameter 2 to be object, array given in %s on line %d
NULL

Arg value Array 

Warning: xml_set_object() expects parameter 2 to be object, array given in %s on line %d
NULL

Arg value Array 

Warning: xml_set_object() expects parameter 2 to be object, array given in %s on line %d
NULL

Arg value  

Warning: xml_set_object() expects parameter 2 to be object, null given in %s on line %d
NULL

Arg value  

Warning: xml_set_object() expects parameter 2 to be object, null given in %s on line %d
NULL

Arg value 1 

Warning: xml_set_object() expects parameter 2 to be object, bool given in %s on line %d
NULL

Arg value  

Warning: xml_set_object() expects parameter 2 to be object, bool given in %s on line %d
NULL

Arg value 1 

Warning: xml_set_object() expects parameter 2 to be object, bool given in %s on line %d
NULL

Arg value  

Warning: xml_set_object() expects parameter 2 to be object, bool given in %s on line %d
NULL

Arg value  

Warning: xml_set_object() expects parameter 2 to be object, string given in %s on line %d
NULL

Arg value  

Warning: xml_set_object() expects parameter 2 to be object, string given in %s on line %d
NULL

Arg value string 

Warning: xml_set_object() expects parameter 2 to be object, string given in %s on line %d
NULL

Arg value string 

Warning: xml_set_object() expects parameter 2 to be object, string given in %s on line %d
NULL

Arg value Resource id %s

Warning: xml_set_object() expects parameter 2 to be object, resource given in %s on line %d
NULL

Arg value  

Warning: xml_set_object() expects parameter 2 to be object, null given in %s on line %d
NULL

Arg value  

Warning: xml_set_object() expects parameter 2 to be object, null given in %s on line %d
NULL
Done
