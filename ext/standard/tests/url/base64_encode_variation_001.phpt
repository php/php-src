--TEST--
Test base64_encode() function : usage variations - unexpected types for argument 1
--FILE--
<?php
/* Prototype  : proto string base64_encode(string str)
 * Description: Encodes string using MIME base64 algorithm
 * Source code: ext/standard/base64.c
 * Alias to functions:
 */

echo "*** Testing base64_encode() : usage variations ***\n";


function test_error_handler($err_no, $err_msg, $filename, $linenum, $vars) {
	echo "Error: $err_no - $err_msg, $filename($linenum)\n";
}
set_error_handler('test_error_handler');

// Initialise function arguments not being substituted (if any)

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

      // object data
      new stdclass(),

      // undefined data
      $undefined_var,

      // unset data
      $unset_var,
);

// loop through each element of the array for str

foreach($values as $value) {
      echo "\nArg value $value \n";
      var_dump( base64_encode($value) );
};

echo "Done";
?>
--EXPECTF--
*** Testing base64_encode() : usage variations ***
Error: 8 - Undefined variable: undefined_var, %s(63)
Error: 8 - Undefined variable: unset_var, %s(66)

Arg value 0 
string(4) "MA=="

Arg value 1 
string(4) "MQ=="

Arg value 12345 
string(8) "MTIzNDU="

Arg value -2345 
string(8) "LTIzNDU="

Arg value 10.5 
string(8) "MTAuNQ=="

Arg value -10.5 
string(8) "LTEwLjU="

Arg value 101234567000 
string(16) "MTAxMjM0NTY3MDAw"

Arg value 1.07654321E-9 
string(20) "MS4wNzY1NDMyMUUtOQ=="

Arg value 0.5 
string(4) "MC41"
Error: 8 - Array to string conversion, %sbase64_encode_variation_001.php(%d)

Arg value Array 
Error: 2 - base64_encode() expects parameter 1 to be string, array given, %s(73)
NULL
Error: 8 - Array to string conversion, %sbase64_encode_variation_001.php(%d)

Arg value Array 
Error: 2 - base64_encode() expects parameter 1 to be string, array given, %s(73)
NULL
Error: 8 - Array to string conversion, %sbase64_encode_variation_001.php(%d)

Arg value Array 
Error: 2 - base64_encode() expects parameter 1 to be string, array given, %s(73)
NULL
Error: 8 - Array to string conversion, %sbase64_encode_variation_001.php(%d)

Arg value Array 
Error: 2 - base64_encode() expects parameter 1 to be string, array given, %s(73)
NULL
Error: 8 - Array to string conversion, %sbase64_encode_variation_001.php(%d)

Arg value Array 
Error: 2 - base64_encode() expects parameter 1 to be string, array given, %s(73)
NULL

Arg value  
string(0) ""

Arg value  
string(0) ""

Arg value 1 
string(4) "MQ=="

Arg value  
string(0) ""

Arg value 1 
string(4) "MQ=="

Arg value  
string(0) ""

Arg value  
string(0) ""

Arg value  
string(0) ""
Error: 4096 - Object of class stdClass could not be converted to string, %s(72)

Arg value  
Error: 2 - base64_encode() expects parameter 1 to be string, object given, %s(73)
NULL

Arg value  
string(0) ""

Arg value  
string(0) ""
Done
