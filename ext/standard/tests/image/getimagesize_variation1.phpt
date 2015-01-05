--TEST--
Test getimagesize() function : usage variations  - unexpected type for arg 1
--FILE--
<?php
/* Prototype  : proto array getimagesize(string imagefile [, array info])
 * Description: Get the size of an image as 4-element array 
 * Source code: ext/standard/image.c
 * Alias to functions: 
 */

function test_error_handler($err_no, $err_msg, $filename, $linenum, $vars) {
	echo "Error: $err_no - $err_msg, $filename($linenum)\n";
}
set_error_handler('test_error_handler');

echo "*** Testing getimagesize() : usage variations ***\n";

// Initialise function arguments not being substituted (if any)
$info = array(1, 2);

//get an unset variable
$unset_var = 10;
unset ($unset_var);

//array of values to iterate over
$values = array(

      // int data
      "0" => 0,
      "1" => 1,
      "12345" => 12345,
      "-2345" => -2345,

      // float data
      "10.5" => 10.5,
      "-10.5" => -10.5,
      "10.1234567e5" => 10.1234567e10,
      "10.7654321e-5" => 10.7654321E-5,
      .5,

      // array data
      "array()" => array(),
      "array(0)" => array(0),
      "array(1)" => array(1),
      "array(1, 2)" => array(1, 2),
      "array('color' => 'red', 'item' => 'pen')" => array('color' => 'red', 'item' => 'pen'),

      // null data
      "NULL" => NULL,
      "null" => null,

      // boolean data
      "true" => true,
      "false" => false,
      "TRUE" => TRUE,
      "FALSE" => FALSE,

      // empty data
      "\"\"" => "",
      "''" => '',

      // object data
      "new stdclass()" => new stdclass(),

      // undefined data
      "undefined_var" => $undefined_var,

      // unset data
      "unset_var" => $unset_var,
);

// loop through each element of the array for imagefile

foreach($values as $key => $value) {
      echo "\n-- Arg value: $key --\n";
      var_dump( getimagesize($value, $info) );
};

?>
===DONE===
--EXPECTF--
*** Testing getimagesize() : usage variations ***
Error: 8 - Undefined variable: undefined_var, %s(%d)
Error: 8 - Undefined variable: unset_var, %s(%d)

-- Arg value: 0 --
Error: 2 - getimagesize(0): failed to open stream: No such file or directory, %s(%d)
bool(false)

-- Arg value: 1 --
Error: 2 - getimagesize(1): failed to open stream: No such file or directory, %s(%d)
bool(false)

-- Arg value: 12345 --
Error: 2 - getimagesize(12345): failed to open stream: No such file or directory, %s(%d)
bool(false)

-- Arg value: -2345 --
Error: 2 - getimagesize(-2345): failed to open stream: No such file or directory, %s(%d)
bool(false)

-- Arg value: 10.5 --
Error: 2 - getimagesize(10.5): failed to open stream: No such file or directory, %s(%d)
bool(false)

-- Arg value: -10.5 --
Error: 2 - getimagesize(-10.5): failed to open stream: No such file or directory, %s(%d)
bool(false)

-- Arg value: 10.1234567e5 --
Error: 2 - getimagesize(101234567000): failed to open stream: No such file or directory, %s(%d)
bool(false)

-- Arg value: 10.7654321e-5 --
Error: 2 - getimagesize(0.000107654321): failed to open stream: No such file or directory, %s(%d)
bool(false)

-- Arg value: 12346 --
Error: 2 - getimagesize(0.5): failed to open stream: No such file or directory, %s(%d)
bool(false)

-- Arg value: array() --
Error: 2 - getimagesize() expects parameter 1 to be string, array given, %s(%d)
NULL

-- Arg value: array(0) --
Error: 2 - getimagesize() expects parameter 1 to be string, array given, %s(%d)
NULL

-- Arg value: array(1) --
Error: 2 - getimagesize() expects parameter 1 to be string, array given, %s(%d)
NULL

-- Arg value: array(1, 2) --
Error: 2 - getimagesize() expects parameter 1 to be string, array given, %s(%d)
NULL

-- Arg value: array('color' => 'red', 'item' => 'pen') --
Error: 2 - getimagesize() expects parameter 1 to be string, array given, %s(%d)
NULL

-- Arg value: NULL --
Error: 2 - getimagesize(): Filename cannot be empty, %s(%d)
bool(false)

-- Arg value: null --
Error: 2 - getimagesize(): Filename cannot be empty, %s(%d)
bool(false)

-- Arg value: true --
Error: 2 - getimagesize(1): failed to open stream: No such file or directory, %s(%d)
bool(false)

-- Arg value: false --
Error: 2 - getimagesize(): Filename cannot be empty, %s(%d)
bool(false)

-- Arg value: TRUE --
Error: 2 - getimagesize(1): failed to open stream: No such file or directory, %s(%d)
bool(false)

-- Arg value: FALSE --
Error: 2 - getimagesize(): Filename cannot be empty, %s(%d)
bool(false)

-- Arg value: "" --
Error: 2 - getimagesize(): Filename cannot be empty, %s(%d)
bool(false)

-- Arg value: '' --
Error: 2 - getimagesize(): Filename cannot be empty, %s(%d)
bool(false)

-- Arg value: new stdclass() --
Error: 2 - getimagesize() expects parameter 1 to be string, object given, %s(%d)
NULL

-- Arg value: undefined_var --
Error: 2 - getimagesize(): Filename cannot be empty, %s(%d)
bool(false)

-- Arg value: unset_var --
Error: 2 - getimagesize(): Filename cannot be empty, %s(%d)
bool(false)
===DONE===