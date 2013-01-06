--TEST--
Test get_parent_class() function : usage variations  - unexpected argument type.
--FILE--
<?php
/* Prototype  : proto string get_parent_class([mixed object])
 * Description: Retrieves the parent class name for object or class or current scope. 
 * Source code: Zend/zend_builtin_functions.c
 * Alias to functions: 
 */

function __autoload($className) {
	echo "In __autoload($className)\n";
}

function test_error_handler($err_no, $err_msg, $filename, $linenum, $vars) {
	echo "Error: $err_no - $err_msg, $filename($linenum)\n";
}
set_error_handler('test_error_handler');

echo "*** Testing get_parent_class() : usage variations ***\n";

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
      'String',

      // object data
      new stdclass(),

      // undefined data
      $undefined_var,

      // unset data
      $unset_var,
);

// loop through each element of the array for object

foreach($values as $value) {
      echo "\nArg value $value \n";
      var_dump( get_parent_class($value) );
};

echo "Done";
?>
--EXPECTF--
*** Testing get_parent_class() : usage variations ***
Error: 8 - Undefined variable: undefined_var, %s(68)
Error: 8 - Undefined variable: unset_var, %s(71)

Arg value 0 
bool(false)

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
Error: 8 - Array to string conversion, %sget_parent_class_variation_002.php(%d)

Arg value Array 
bool(false)
Error: 8 - Array to string conversion, %sget_parent_class_variation_002.php(%d)

Arg value Array 
bool(false)
Error: 8 - Array to string conversion, %sget_parent_class_variation_002.php(%d)

Arg value Array 
bool(false)
Error: 8 - Array to string conversion, %sget_parent_class_variation_002.php(%d)

Arg value Array 
bool(false)
Error: 8 - Array to string conversion, %sget_parent_class_variation_002.php(%d)

Arg value Array 
bool(false)

Arg value  
bool(false)

Arg value  
bool(false)

Arg value 1 
bool(false)

Arg value  
bool(false)

Arg value 1 
bool(false)

Arg value  
bool(false)

Arg value  
bool(false)

Arg value  
bool(false)

Arg value string 
In __autoload(string)
bool(false)

Arg value String 
In __autoload(String)
bool(false)
Error: 4096 - Object of class stdClass could not be converted to string, %s(77)

Arg value  
bool(false)

Arg value  
bool(false)

Arg value  
bool(false)
Done