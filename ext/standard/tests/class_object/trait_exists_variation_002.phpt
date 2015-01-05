--TEST--
Test trait_exists() function : usage variations  - unexpected types for argument 2
--FILE--
<?php
/* Prototype  : proto bool trait_exists(string traitname [, bool autoload])
 * Description: Checks if the trait exists 
 * Source code: Zend/zend_builtin_functions.c
 * Alias to functions: 
 */

function __autoload($traitName) {
	echo "In __autoload($traitName)\n";
}

function test_error_handler($err_no, $err_msg, $filename, $linenum, $vars) {
	echo "Error: $err_no - $err_msg, $filename($linenum)\n";
}
set_error_handler('test_error_handler');

echo "*** Testing trait_exists() : usage variations ***\n";

// Initialise function arguments not being substituted (if any)
$traitname = 'string_val';

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

// loop through each element of the array for autoload

foreach($values as $value) {
      echo "\nArg value $value \n";
      var_dump( trait_exists($traitname, $value) );
};

echo "Done";
?>
--EXPECTF--
*** Testing trait_exists() : usage variations ***
Error: 8 - Undefined variable: undefined_var, %s(71)
Error: 8 - Undefined variable: unset_var, %s(74)

Arg value 0 
bool(false)

Arg value 1 
In __autoload(string_val)
bool(false)

Arg value 12345 
In __autoload(string_val)
bool(false)

Arg value -2345 
In __autoload(string_val)
bool(false)

Arg value 10.5 
In __autoload(string_val)
bool(false)

Arg value -10.5 
In __autoload(string_val)
bool(false)

Arg value 101234567000 
In __autoload(string_val)
bool(false)

Arg value 1.07654321E-9 
In __autoload(string_val)
bool(false)

Arg value 0.5 
In __autoload(string_val)
bool(false)
Error: 8 - Array to string conversion, %strait_exists_variation_002.php(%d)

Arg value Array 
Error: 2 - trait_exists() expects parameter 2 to be boolean, array given, %s(81)
NULL
Error: 8 - Array to string conversion, %strait_exists_variation_002.php(%d)

Arg value Array 
Error: 2 - trait_exists() expects parameter 2 to be boolean, array given, %s(81)
NULL
Error: 8 - Array to string conversion, %strait_exists_variation_002.php(%d)

Arg value Array 
Error: 2 - trait_exists() expects parameter 2 to be boolean, array given, %s(81)
NULL
Error: 8 - Array to string conversion, %strait_exists_variation_002.php(%d)

Arg value Array 
Error: 2 - trait_exists() expects parameter 2 to be boolean, array given, %s(81)
NULL
Error: 8 - Array to string conversion, %strait_exists_variation_002.php(%d)

Arg value Array 
Error: 2 - trait_exists() expects parameter 2 to be boolean, array given, %s(81)
NULL

Arg value  
bool(false)

Arg value  
bool(false)

Arg value 1 
In __autoload(string_val)
bool(false)

Arg value  
bool(false)

Arg value 1 
In __autoload(string_val)
bool(false)

Arg value  
bool(false)

Arg value  
bool(false)

Arg value  
bool(false)

Arg value string 
In __autoload(string_val)
bool(false)

Arg value string 
In __autoload(string_val)
bool(false)
Error: 4096 - Object of class stdClass could not be converted to string, %s(80)

Arg value  
Error: 2 - trait_exists() expects parameter 2 to be boolean, object given, %s(81)
NULL

Arg value  
bool(false)

Arg value  
bool(false)
Done