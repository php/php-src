--TEST--
Test xml_error_string() function : usage variations  - test different types for code
--SKIPIF--
<?php if (PHP_INT_SIZE != 8) die("skip this test is for 64-bit only");
if (!extension_loaded("xml")) {
	print "skip - XML extension not loaded";
}
?>
--FILE--
<?php
/* Prototype  : proto string xml_error_string(int code)
 * Description: Get XML parser error string
 * Source code: ext/xml/xml.c
 * Alias to functions:
 */

echo "*** Testing xml_error_string() : usage variations ***\n";
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

//array of values to iterate over
$values = array(

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

// loop through each element of the array for code

foreach($values as $value) {
      echo @"\nArg value $value \n";
      var_dump( xml_error_string($value) );
};

echo "Done";
?>
--EXPECTF--
*** Testing xml_error_string() : usage variations ***

Arg value 10.5 
string(22) "XML_ERR_CHARREF_AT_EOF"

Arg value -10.5 
string(7) "Unknown"

Arg value 101234567000 
string(7) "Unknown"

Arg value 1.07654321E-9 
string(8) "No error"

Arg value 0.5 
string(8) "No error"

Arg value Array 

Warning: xml_error_string() expects parameter 1 to be integer, array given in %s on line %d
NULL

Arg value Array 

Warning: xml_error_string() expects parameter 1 to be integer, array given in %s on line %d
NULL

Arg value Array 

Warning: xml_error_string() expects parameter 1 to be integer, array given in %s on line %d
NULL

Arg value Array 

Warning: xml_error_string() expects parameter 1 to be integer, array given in %s on line %d
NULL

Arg value Array 

Warning: xml_error_string() expects parameter 1 to be integer, array given in %s on line %d
NULL

Arg value  
string(8) "No error"

Arg value  
string(8) "No error"

Arg value 1 
string(9) "No memory"

Arg value  
string(8) "No error"

Arg value 1 
string(9) "No memory"

Arg value  
string(8) "No error"

Arg value  

Warning: xml_error_string() expects parameter 1 to be integer, string given in %s on line %d
NULL

Arg value  

Warning: xml_error_string() expects parameter 1 to be integer, string given in %s on line %d
NULL

Arg value string 

Warning: xml_error_string() expects parameter 1 to be integer, string given in %s on line %d
NULL

Arg value string 

Warning: xml_error_string() expects parameter 1 to be integer, string given in %s on line %d
NULL

Arg value Some Ascii Data 

Warning: xml_error_string() expects parameter 1 to be integer, object given in %s on line %d
NULL

Arg value  
string(8) "No error"

Arg value  
string(8) "No error"
Done
