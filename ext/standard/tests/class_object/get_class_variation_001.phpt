--TEST--
Test get_class() function : usage variations  - passing unexpected types.
--FILE--
<?php
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
    echo @"\nArg value: $value (type: " . gettype($value) . ")\n";
    try {
        var_dump( get_class($value) );
    } catch (TypeError $e) {
        echo $e->getMessage(), "\n";
    }
};

echo "Done";
?>
--EXPECTF--
*** Testing get_class() : usage variations ***

Warning: Undefined variable $undefined_var in %s on line %d

Warning: Undefined variable $unset_var in %s on line %d

Arg value: 0 (type: integer)
get_class(): Argument #1 ($object) must be of type object, int given

Arg value: 1 (type: integer)
get_class(): Argument #1 ($object) must be of type object, int given

Arg value: 12345 (type: integer)
get_class(): Argument #1 ($object) must be of type object, int given

Arg value: -2345 (type: integer)
get_class(): Argument #1 ($object) must be of type object, int given

Arg value: 10.5 (type: double)
get_class(): Argument #1 ($object) must be of type object, float given

Arg value: -10.5 (type: double)
get_class(): Argument #1 ($object) must be of type object, float given

Arg value: 101234567000 (type: double)
get_class(): Argument #1 ($object) must be of type object, float given

Arg value: 1.07654321E-9 (type: double)
get_class(): Argument #1 ($object) must be of type object, float given

Arg value: 0.5 (type: double)
get_class(): Argument #1 ($object) must be of type object, float given

Arg value: Array (type: array)
get_class(): Argument #1 ($object) must be of type object, array given

Arg value: Array (type: array)
get_class(): Argument #1 ($object) must be of type object, array given

Arg value: Array (type: array)
get_class(): Argument #1 ($object) must be of type object, array given

Arg value: Array (type: array)
get_class(): Argument #1 ($object) must be of type object, array given

Arg value: Array (type: array)
get_class(): Argument #1 ($object) must be of type object, array given

Arg value:  (type: NULL)
get_class(): Argument #1 ($object) must be of type object, null given

Arg value:  (type: NULL)
get_class(): Argument #1 ($object) must be of type object, null given

Arg value: 1 (type: boolean)
get_class(): Argument #1 ($object) must be of type object, true given

Arg value:  (type: boolean)
get_class(): Argument #1 ($object) must be of type object, false given

Arg value: 1 (type: boolean)
get_class(): Argument #1 ($object) must be of type object, true given

Arg value:  (type: boolean)
get_class(): Argument #1 ($object) must be of type object, false given

Arg value:  (type: string)
get_class(): Argument #1 ($object) must be of type object, string given

Arg value:  (type: string)
get_class(): Argument #1 ($object) must be of type object, string given

Arg value: string (type: string)
get_class(): Argument #1 ($object) must be of type object, string given

Arg value: string (type: string)
get_class(): Argument #1 ($object) must be of type object, string given

Arg value:  (type: NULL)
get_class(): Argument #1 ($object) must be of type object, null given

Arg value:  (type: NULL)
get_class(): Argument #1 ($object) must be of type object, null given
Done
