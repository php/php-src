--TEST--
Test xml_parser_set_option() function : usage variations
--SKIPIF--
<?php
if (PHP_INT_SIZE != 8) die("skip this test is for 64-bit only");
if (!extension_loaded("xml")) {
	print "skip - XML extension not loaded";
}
?>
--FILE--
<?php
/* Prototype  : proto int xml_parser_set_option(resource parser, int option, mixed value)
 * Description: Set options in an XML parser
 * Source code: ext/xml/xml.c
 * Alias to functions:
 */

echo "*** Testing xml_parser_set_option() : usage variations ***\n";
error_reporting(E_ALL & ~E_NOTICE);

class aClass {
   function __toString() {
       return "Some Ascii Data";
   }
}
// Initialise function arguments not being substituted (if any)
$parser = xml_parser_create();

//get an unset variable
$unset_var = 10;
unset ($unset_var);

//array of values to iterate over
$values = array(

      // outside of range int data
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

      // undefined data
      $undefined_var,

      // unset data
      $unset_var,
);

// loop through each element of the array for option

foreach($values as $value) {
      echo @"\nArg value $value \n";
      var_dump( xml_parser_set_option($parser, $value, 1) );
};

xml_parser_free($parser);
echo "Done";
?>
--EXPECTF--
*** Testing xml_parser_set_option() : usage variations ***

Arg value 12345 

Warning: xml_parser_set_option(): Unknown option in %s on line %d
bool(false)

Arg value -2345 

Warning: xml_parser_set_option(): Unknown option in %s on line %d
bool(false)

Arg value 10.5 

Warning: xml_parser_set_option(): Unknown option in %s on line %d
bool(false)

Arg value -10.5 

Warning: xml_parser_set_option(): Unknown option in %s on line %d
bool(false)

Arg value 101234567000 

Warning: xml_parser_set_option(): Unknown option in %s on line %d
bool(false)

Arg value 1.07654321E-9 

Warning: xml_parser_set_option(): Unknown option in %s on line %d
bool(false)

Arg value 0.5 

Warning: xml_parser_set_option(): Unknown option in %s on line %d
bool(false)

Arg value Array 

Warning: xml_parser_set_option() expects parameter 2 to be int, array given in %s on line %d
NULL

Arg value Array 

Warning: xml_parser_set_option() expects parameter 2 to be int, array given in %s on line %d
NULL

Arg value Array 

Warning: xml_parser_set_option() expects parameter 2 to be int, array given in %s on line %d
NULL

Arg value Array 

Warning: xml_parser_set_option() expects parameter 2 to be int, array given in %s on line %d
NULL

Arg value Array 

Warning: xml_parser_set_option() expects parameter 2 to be int, array given in %s on line %d
NULL

Arg value  

Warning: xml_parser_set_option(): Unknown option in %s on line %d
bool(false)

Arg value  

Warning: xml_parser_set_option(): Unknown option in %s on line %d
bool(false)

Arg value 1 
bool(true)

Arg value  

Warning: xml_parser_set_option(): Unknown option in %s on line %d
bool(false)

Arg value 1 
bool(true)

Arg value  

Warning: xml_parser_set_option(): Unknown option in %s on line %d
bool(false)

Arg value  

Warning: xml_parser_set_option() expects parameter 2 to be int, string given in %s on line %d
NULL

Arg value  

Warning: xml_parser_set_option() expects parameter 2 to be int, string given in %s on line %d
NULL

Arg value string 

Warning: xml_parser_set_option() expects parameter 2 to be int, string given in %s on line %d
NULL

Arg value string 

Warning: xml_parser_set_option() expects parameter 2 to be int, string given in %s on line %d
NULL

Arg value Some Ascii Data 

Warning: xml_parser_set_option() expects parameter 2 to be int, object given in %s on line %d
NULL

Arg value  

Warning: xml_parser_set_option(): Unknown option in %s on line %d
bool(false)

Arg value  

Warning: xml_parser_set_option(): Unknown option in %s on line %d
bool(false)
Done
