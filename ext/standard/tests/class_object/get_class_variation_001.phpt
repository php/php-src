--TEST--
Test get_class() function : usage variations  - passing unexpected types.
--FILE--
<?php
/* Prototype  : proto string get_class([object object])
 * Description: Retrieves the class name 
 * Source code: Zend/zend_builtin_functions.c
 * Alias to functions: 
 */

echo "*** Testing get_class() : usage variations ***\n";

//  Note: basic use cases in Zend/tests/009.phpt

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

// loop through each element of the array for object

foreach($values as $value) {
      echo "\nArg value: $value (type: " . gettype($value) . ")\n";
      var_dump( get_class($value) );
};

echo "Done";
?>
--EXPECTF--
*** Testing get_class() : usage variations ***

Notice: Undefined variable: undefined_var in %s on line %d

Notice: Undefined variable: unset_var in %s on line %d

Arg value: 0 (type: integer)

Warning: get_class() expects parameter 1 to be object, integer given in %s on line %d
NULL

Arg value: 1 (type: integer)

Warning: get_class() expects parameter 1 to be object, integer given in %s on line %d
NULL

Arg value: 12345 (type: integer)

Warning: get_class() expects parameter 1 to be object, integer given in %s on line %d
NULL

Arg value: -2345 (type: integer)

Warning: get_class() expects parameter 1 to be object, integer given in %s on line %d
NULL

Arg value: 10.5 (type: double)

Warning: get_class() expects parameter 1 to be object, double given in %s on line %d
NULL

Arg value: -10.5 (type: double)

Warning: get_class() expects parameter 1 to be object, double given in %s on line %d
NULL

Arg value: 101234567000 (type: double)

Warning: get_class() expects parameter 1 to be object, double given in %s on line %d
NULL

Arg value: 1.07654321E-9 (type: double)

Warning: get_class() expects parameter 1 to be object, double given in %s on line %d
NULL

Arg value: 0.5 (type: double)

Warning: get_class() expects parameter 1 to be object, double given in %s on line %d
NULL

Notice: Array to string conversion in %s on line %d

Arg value: Array (type: array)

Warning: get_class() expects parameter 1 to be object, array given in %s on line %d
NULL

Notice: Array to string conversion in %s on line %d

Arg value: Array (type: array)

Warning: get_class() expects parameter 1 to be object, array given in %s on line %d
NULL

Notice: Array to string conversion in %s on line %d

Arg value: Array (type: array)

Warning: get_class() expects parameter 1 to be object, array given in %s on line %d
NULL

Notice: Array to string conversion in %s on line %d

Arg value: Array (type: array)

Warning: get_class() expects parameter 1 to be object, array given in %s on line %d
NULL

Notice: Array to string conversion in %s on line %d

Arg value: Array (type: array)

Warning: get_class() expects parameter 1 to be object, array given in %s on line %d
NULL

Arg value:  (type: NULL)

Warning: get_class() expects parameter 1 to be object, null given in %s on line %d
NULL

Arg value:  (type: NULL)

Warning: get_class() expects parameter 1 to be object, null given in %s on line %d
NULL

Arg value: 1 (type: boolean)

Warning: get_class() expects parameter 1 to be object, boolean given in %s on line %d
NULL

Arg value:  (type: boolean)

Warning: get_class() expects parameter 1 to be object, boolean given in %s on line %d
NULL

Arg value: 1 (type: boolean)

Warning: get_class() expects parameter 1 to be object, boolean given in %s on line %d
NULL

Arg value:  (type: boolean)

Warning: get_class() expects parameter 1 to be object, boolean given in %s on line %d
NULL

Arg value:  (type: unicode)

Warning: get_class() expects parameter 1 to be object, Unicode string given in %s on line %d
NULL

Arg value:  (type: unicode)

Warning: get_class() expects parameter 1 to be object, Unicode string given in %s on line %d
NULL

Arg value: string (type: unicode)

Warning: get_class() expects parameter 1 to be object, Unicode string given in %s on line %d
NULL

Arg value: string (type: unicode)

Warning: get_class() expects parameter 1 to be object, Unicode string given in %s on line %d
NULL

Arg value:  (type: NULL)

Warning: get_class() expects parameter 1 to be object, null given in %s on line %d
NULL

Arg value:  (type: NULL)

Warning: get_class() expects parameter 1 to be object, null given in %s on line %d
NULL
Done
