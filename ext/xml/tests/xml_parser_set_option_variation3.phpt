--TEST--
Test xml_parser_set_option() function : usage variations
--SKIPIF--
<?php
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
$option = 1;

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

// loop through each element of the array for value

foreach($values as $value) {
      echo @"\nArg value $value \n";
      var_dump( xml_parser_set_option($parser, $option, $value) );
};

fclose($fp);
xml_parser_free($parser);
echo "Done";
?>
--EXPECTF--
*** Testing xml_parser_set_option() : usage variations ***

Arg value 0 
bool(true)

Arg value 1 
bool(true)

Arg value 12345 
bool(true)

Arg value -2345 
bool(true)

Arg value 10.5 
bool(true)

Arg value -10.5 
bool(true)

Arg value 101234567000 
bool(true)

Arg value 1.07654321E-9 
bool(true)

Arg value 0.5 
bool(true)

Arg value Array 
bool(true)

Arg value Array 
bool(true)

Arg value Array 
bool(true)

Arg value Array 
bool(true)

Arg value Array 
bool(true)

Arg value  
bool(true)

Arg value  
bool(true)

Arg value 1 
bool(true)

Arg value  
bool(true)

Arg value 1 
bool(true)

Arg value  
bool(true)

Arg value  
bool(true)

Arg value  
bool(true)

Arg value string 
bool(true)

Arg value string 
bool(true)

Arg value Some Ascii Data 
bool(true)

Arg value Resource id %s
bool(true)

Arg value  
bool(true)

Arg value  
bool(true)
Done
