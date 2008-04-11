--TEST--
Test base64_decode() function : usage variations   - unexpected types for arg 2
--FILE--
<?php
/* Prototype  : proto string base64_decode(string str[, bool strict])
 * Description: Decodes string using MIME base64 algorithm 
 * Source code: ext/standard/base64.c
 * Alias to functions: 
 */

function test_error_handler($err_no, $err_msg, $filename, $linenum, $vars) {
	echo "Error: $err_no - $err_msg, $filename($linenum)\n";
}
set_error_handler('test_error_handler');

echo "*** Testing base64_decode() : usage variations ***\n";

// Initialise function arguments not being substituted (if any)
$str = 'aGVsbG8gd29ybGQh!';

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

      // object data
      new stdclass(),

      // undefined data
      $undefined_var,

      // unset data
      $unset_var,
);

// loop through each element of the array for strict

foreach($values as $value) {
      echo "\nArg value $value \n";
      var_dump( base64_decode($str, $value) );
};

echo "Done";
?>
--EXPECTF--
*** Testing base64_decode() : usage variations ***
Error: 8 - Undefined variable: undefined_var, %s(67)
Error: 8 - Undefined variable: unset_var, %s(70)

Arg value 0 
string(12) "hello world!"

Arg value 1 
bool(false)

Arg value 12345 
bool(false)

Arg value -2345 
bool(false)

Arg value 10.5 
bool(false)

Arg value -10.5 
bool(false)

Arg value 101234567000 
bool(false)

Arg value 1.07654321E-9 
bool(false)

Arg value 0.5 
bool(false)

Arg value Array 
Error: 2 - base64_decode() expects parameter 2 to be boolean, array given, %s(77)
NULL

Arg value Array 
Error: 2 - base64_decode() expects parameter 2 to be boolean, array given, %s(77)
NULL

Arg value Array 
Error: 2 - base64_decode() expects parameter 2 to be boolean, array given, %s(77)
NULL

Arg value Array 
Error: 2 - base64_decode() expects parameter 2 to be boolean, array given, %s(77)
NULL

Arg value Array 
Error: 2 - base64_decode() expects parameter 2 to be boolean, array given, %s(77)
NULL

Arg value  
string(12) "hello world!"

Arg value  
string(12) "hello world!"

Arg value 1 
bool(false)

Arg value  
string(12) "hello world!"

Arg value 1 
bool(false)

Arg value  
string(12) "hello world!"

Arg value  
string(12) "hello world!"

Arg value  
string(12) "hello world!"

Arg value string 
bool(false)

Arg value string 
bool(false)
Error: 4096 - Object of class stdClass could not be converted to string, %s(76)

Arg value  
Error: 2 - base64_decode() expects parameter 2 to be boolean, object given, %s(77)
NULL

Arg value  
string(12) "hello world!"

Arg value  
string(12) "hello world!"
Done