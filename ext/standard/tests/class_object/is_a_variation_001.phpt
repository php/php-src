--TEST--
Test is_a() function : usage variations  - wrong type for arg 1
--INI--
error_reporting=E_ALL | E_STRICT | E_DEPRECATED
--FILE--
<?php
/* Prototype  : proto bool is_a(object object, string class_name)
 * Description: Returns true if the object is of this class or has this class as one of its parents
 * Source code: Zend/zend_builtin_functions.c
 * Alias to functions:
 */
// Note: basic use cases in Zend/tests/is_a.phpt
echo "*** Testing is_a() : usage variations ***\n";

// Initialise function arguments not being substituted (if any)
$class_name = 'stdClass';

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

      // undefined data
      $undefined_var,

      // unset data
      $unset_var,
);

// loop through each element of the array for object

foreach($values as $value) {
      echo @"\nArg value $value \n";
      var_dump( is_a($value, $class_name) );
};

echo "Done";
?>
--EXPECTF--
*** Testing is_a() : usage variations ***

Notice: Undefined variable: undefined_var in %s on line 59

Notice: Undefined variable: unset_var in %s on line 62

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

Arg value Array 
bool(false)

Arg value Array 
bool(false)

Arg value Array 
bool(false)

Arg value Array 
bool(false)

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
bool(false)

Arg value String 
bool(false)

Arg value  
bool(false)

Arg value  
bool(false)
Done
