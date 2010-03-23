--TEST--
Test get_class_methods() function : usage variations  - unexpected types
--FILE--
<?php
/* Prototype  : proto array get_class_methods(mixed class)
 * Description: Returns an array of method names for class or class instance. 
 * Source code: Zend/zend_builtin_functions.c
 * Alias to functions: 
 */


function test_error_handler($err_no, $err_msg, $filename, $linenum, $vars) {
	echo "Error: $err_no - $err_msg, $filename($linenum)\n";
}
set_error_handler('test_error_handler');

echo "*** Testing get_class_methods() : usage variations ***\n";

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

// loop through each element of the array for class

foreach($values as $value) {
      echo "\nArg value $value \n";
      var_dump( get_class_methods($value) );
};

echo "Done";
?>
--EXPECTF--
*** Testing get_class_methods() : usage variations ***
Error: 8 - Undefined variable: undefined_var, %s(67)
Error: 8 - Undefined variable: unset_var, %s(70)

Arg value 0 
NULL

Arg value 1 
NULL

Arg value 12345 
NULL

Arg value -2345 
NULL

Arg value 10.5 
NULL

Arg value -10.5 
NULL

Arg value 101234567000 
NULL

Arg value 1.07654321E-9 
NULL

Arg value 0.5 
NULL

Arg value Array 
NULL

Arg value Array 
NULL

Arg value Array 
NULL

Arg value Array 
NULL

Arg value Array 
NULL

Arg value  
NULL

Arg value  
NULL

Arg value 1 
NULL

Arg value  
NULL

Arg value 1 
NULL

Arg value  
NULL

Arg value  
NULL

Arg value  
NULL

Arg value string 
NULL

Arg value string 
NULL
Error: 4096 - Object of class stdClass could not be converted to string, %s(76)

Arg value  
array(0) {
}

Arg value  
NULL

Arg value  
NULL
Done