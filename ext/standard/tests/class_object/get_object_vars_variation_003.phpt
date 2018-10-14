--TEST--
Test get_object_vars() function : usage variations  - unexpected types for argument 1
--FILE--
<?php
/* Prototype  : proto array get_object_vars(object obj)
 * Description: Returns an array of object properties
 * Source code: Zend/zend_builtin_functions.c
 * Alias to functions:
 */

echo "*** Testing get_object_vars() : usage variations ***\n";

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

      // undefined data
      $undefined_var,

      // unset data
      $unset_var,
);

// loop through each element of the array for obj

foreach($values as $value) {
      echo @"\nArg value $value \n";
      var_dump( get_object_vars($value) );
};

echo "Done";
?>
--EXPECTF--
*** Testing get_object_vars() : usage variations ***

Notice: Undefined variable: undefined_var in %s on line 56

Notice: Undefined variable: unset_var in %s on line 59

Arg value 0 

Warning: get_object_vars() expects parameter 1 to be object, integer given in %s on line %d
NULL

Arg value 1 

Warning: get_object_vars() expects parameter 1 to be object, integer given in %s on line %d
NULL

Arg value 12345 

Warning: get_object_vars() expects parameter 1 to be object, integer given in %s on line %d
NULL

Arg value -2345 

Warning: get_object_vars() expects parameter 1 to be object, integer given in %s on line %d
NULL

Arg value 10.5 

Warning: get_object_vars() expects parameter 1 to be object, float given in %s on line %d
NULL

Arg value -10.5 

Warning: get_object_vars() expects parameter 1 to be object, float given in %s on line %d
NULL

Arg value 101234567000 

Warning: get_object_vars() expects parameter 1 to be object, float given in %s on line %d
NULL

Arg value 1.07654321E-9 

Warning: get_object_vars() expects parameter 1 to be object, float given in %s on line %d
NULL

Arg value 0.5 

Warning: get_object_vars() expects parameter 1 to be object, float given in %s on line %d
NULL

Arg value Array 

Warning: get_object_vars() expects parameter 1 to be object, array given in %s on line %d
NULL

Arg value Array 

Warning: get_object_vars() expects parameter 1 to be object, array given in %s on line %d
NULL

Arg value Array 

Warning: get_object_vars() expects parameter 1 to be object, array given in %s on line %d
NULL

Arg value Array 

Warning: get_object_vars() expects parameter 1 to be object, array given in %s on line %d
NULL

Arg value Array 

Warning: get_object_vars() expects parameter 1 to be object, array given in %s on line %d
NULL

Arg value  

Warning: get_object_vars() expects parameter 1 to be object, null given in %s on line %d
NULL

Arg value  

Warning: get_object_vars() expects parameter 1 to be object, null given in %s on line %d
NULL

Arg value 1 

Warning: get_object_vars() expects parameter 1 to be object, boolean given in %s on line %d
NULL

Arg value  

Warning: get_object_vars() expects parameter 1 to be object, boolean given in %s on line %d
NULL

Arg value 1 

Warning: get_object_vars() expects parameter 1 to be object, boolean given in %s on line %d
NULL

Arg value  

Warning: get_object_vars() expects parameter 1 to be object, boolean given in %s on line %d
NULL

Arg value  

Warning: get_object_vars() expects parameter 1 to be object, string given in %s on line %d
NULL

Arg value  

Warning: get_object_vars() expects parameter 1 to be object, string given in %s on line %d
NULL

Arg value string 

Warning: get_object_vars() expects parameter 1 to be object, string given in %s on line %d
NULL

Arg value string 

Warning: get_object_vars() expects parameter 1 to be object, string given in %s on line %d
NULL

Arg value  

Warning: get_object_vars() expects parameter 1 to be object, null given in %s on line %d
NULL

Arg value  

Warning: get_object_vars() expects parameter 1 to be object, null given in %s on line %d
NULL
Done
