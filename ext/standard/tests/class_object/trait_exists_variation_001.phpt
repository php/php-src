--TEST--
Test trait_exists() function : usage variations - unexpected types for argument 1
--FILE--
<?php
/* Prototype  : proto bool trait_exists(string traitname [, bool autoload])
 * Description: Checks if the trait exists
 * Source code: Zend/zend_builtin_functions.c
 * Alias to functions:
 */

spl_autoload_register(function ($traitName) {
	echo "In autoload($traitName)\n";
});

function test_error_handler($err_no, $err_msg, $filename, $linenum, $vars) {
	echo "Error: $err_no - $err_msg, $filename($linenum)\n";
}
set_error_handler('test_error_handler');

echo "*** Testing trait_exists() : usage variations ***\n";

// Initialise function arguments not being substituted (if any)
$autoload = true;

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

// loop through each element of the array for traitname

foreach($values as $value) {
      echo "\nArg value $value \n";
      var_dump( trait_exists($value, $autoload) );
};

echo "Done";
?>
--EXPECTF--
*** Testing trait_exists() : usage variations ***
Error: 8 - Undefined variable: undefined_var, %s(67)
Error: 8 - Undefined variable: unset_var, %s(70)

Arg value 0 
In autoload(0)
bool(false)

Arg value 1 
In autoload(1)
bool(false)

Arg value 12345 
In autoload(12345)
bool(false)

Arg value -2345 
bool(false)

Arg value 10.5 
bool(false)

Arg value -10.5 
bool(false)

Arg value 101234567000 
In autoload(101234567000)
bool(false)

Arg value 1.07654321E-9 
bool(false)

Arg value 0.5 
bool(false)
Error: 8 - Array to string conversion, %strait_exists_variation_001.php(%d)

Arg value Array 
Error: 2 - trait_exists() expects parameter 1 to be string, array given, %s(77)
NULL
Error: 8 - Array to string conversion, %strait_exists_variation_001.php(%d)

Arg value Array 
Error: 2 - trait_exists() expects parameter 1 to be string, array given, %s(77)
NULL
Error: 8 - Array to string conversion, %strait_exists_variation_001.php(%d)

Arg value Array 
Error: 2 - trait_exists() expects parameter 1 to be string, array given, %s(77)
NULL
Error: 8 - Array to string conversion, %strait_exists_variation_001.php(%d)

Arg value Array 
Error: 2 - trait_exists() expects parameter 1 to be string, array given, %s(77)
NULL
Error: 8 - Array to string conversion, %strait_exists_variation_001.php(%d)

Arg value Array 
Error: 2 - trait_exists() expects parameter 1 to be string, array given, %s(77)
NULL

Arg value  
bool(false)

Arg value  
bool(false)

Arg value 1 
In autoload(1)
bool(false)

Arg value  
bool(false)

Arg value 1 
In autoload(1)
bool(false)

Arg value  
bool(false)

Arg value  
bool(false)

Arg value  
bool(false)
Error: 4096 - Object of class stdClass could not be converted to string, %s(76)

Arg value  
Error: 2 - trait_exists() expects parameter 1 to be string, object given, %s(77)
NULL

Arg value  
bool(false)

Arg value  
bool(false)
Done
