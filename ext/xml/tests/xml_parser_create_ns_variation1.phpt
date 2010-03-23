--TEST--
Test xml_parser_create_ns() function : usage variations
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

echo "*** Testing xml_parser_create_ns() : usage variations ***\n";
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
      "ISO-8859-1",
      "UTF-8",
      "US-ASCII",
      "UTF-32",

      // object data
      new aClass(),
      
      // resource data
      $fp, 

      // undefined data
      $undefined_var,

      // unset data
      $unset_var,
);

// loop through each element of the array for encoding

foreach($values as $value) {
      echo "\nArg value $value \n";
      $res = xml_parser_create_ns($value); 
      var_dump($res);
      if ($res !== false) {
         xml_parser_free($res);
      }
};

fclose($fp);
echo "Done";
?>
--EXPECTF--
*** Testing xml_parser_create_ns() : usage variations ***

Arg value 0 

Warning: xml_parser_create_ns(): unsupported source encoding "0" in %s on line %d
bool(false)

Arg value 1 

Warning: xml_parser_create_ns(): unsupported source encoding "1" in %s on line %d
bool(false)

Arg value 12345 

Warning: xml_parser_create_ns(): unsupported source encoding "12345" in %s on line %d
bool(false)

Arg value -2345 

Warning: xml_parser_create_ns(): unsupported source encoding "-2345" in %s on line %d
bool(false)

Arg value 10.5 

Warning: xml_parser_create_ns(): unsupported source encoding "10.5" in %s on line %d
bool(false)

Arg value -10.5 

Warning: xml_parser_create_ns(): unsupported source encoding "-10.5" in %s on line %d
bool(false)

Arg value 101234567000 

Warning: xml_parser_create_ns(): unsupported source encoding "101234567000" in %s on line %d
bool(false)

Arg value 1.07654321E-9 

Warning: xml_parser_create_ns(): unsupported source encoding "1.07654321E-9" in %s on line %d
bool(false)

Arg value 0.5 

Warning: xml_parser_create_ns(): unsupported source encoding "0.5" in %s on line %d
bool(false)

Arg value Array 

Warning: xml_parser_create_ns() expects parameter 1 to be string, array given in %s on line %d
bool(false)

Arg value Array 

Warning: xml_parser_create_ns() expects parameter 1 to be string, array given in %s on line %d
bool(false)

Arg value Array 

Warning: xml_parser_create_ns() expects parameter 1 to be string, array given in %s on line %d
bool(false)

Arg value Array 

Warning: xml_parser_create_ns() expects parameter 1 to be string, array given in %s on line %d
bool(false)

Arg value Array 

Warning: xml_parser_create_ns() expects parameter 1 to be string, array given in %s on line %d
bool(false)

Arg value  
resource(%d) of type (xml)

Arg value  
resource(%d) of type (xml)

Arg value 1 

Warning: xml_parser_create_ns(): unsupported source encoding "1" in %s on line %d
bool(false)

Arg value  
resource(%d) of type (xml)

Arg value 1 

Warning: xml_parser_create_ns(): unsupported source encoding "1" in %s on line %d
bool(false)

Arg value  
resource(%d) of type (xml)

Arg value  
resource(%d) of type (xml)

Arg value  
resource(%d) of type (xml)

Arg value string 

Warning: xml_parser_create_ns(): unsupported source encoding "string" in %s on line %d
bool(false)

Arg value string 

Warning: xml_parser_create_ns(): unsupported source encoding "string" in %s on line %d
bool(false)

Arg value ISO-8859-1 
resource(%d) of type (xml)

Arg value UTF-8 
resource(%d) of type (xml)

Arg value US-ASCII 
resource(%d) of type (xml)

Arg value UTF-32 

Warning: xml_parser_create_ns(): unsupported source encoding "UTF-32" in %s on line %d
bool(false)

Arg value Some Ascii Data 

Warning: xml_parser_create_ns(): unsupported source encoding "Some Ascii Data" in %s on line %d
bool(false)

Arg value Resource id %s

Warning: xml_parser_create_ns() expects parameter 1 to be string, resource given in %s on line %d
bool(false)

Arg value  
resource(%d) of type (xml)

Arg value  
resource(%d) of type (xml)
Done
