--TEST--
Test get_class_methods() function : usage variations  - unexpected types
--FILE--
<?php
function test_error_handler($err_no, $err_msg, $filename, $linenum) {
    echo "Error: $err_no - $err_msg\n";
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
      echo "\nArg value " . (is_object($value) ? get_class($value) : $value) . "\n";
      try {
            var_dump( get_class_methods($value) );
      } catch (TypeError $exception) {
            echo $exception->getMessage() . "\n";
      }
}
echo "Done";
?>
--EXPECT--
*** Testing get_class_methods() : usage variations ***
Error: 2 - Undefined variable $undefined_var
Error: 2 - Undefined variable $unset_var

Arg value 0
get_class_methods(): Argument #1 ($object_or_class) must be an object or a valid class name, int given

Arg value 1
get_class_methods(): Argument #1 ($object_or_class) must be an object or a valid class name, int given

Arg value 12345
get_class_methods(): Argument #1 ($object_or_class) must be an object or a valid class name, int given

Arg value -2345
get_class_methods(): Argument #1 ($object_or_class) must be an object or a valid class name, int given

Arg value 10.5
get_class_methods(): Argument #1 ($object_or_class) must be an object or a valid class name, float given

Arg value -10.5
get_class_methods(): Argument #1 ($object_or_class) must be an object or a valid class name, float given

Arg value 101234567000
get_class_methods(): Argument #1 ($object_or_class) must be an object or a valid class name, float given

Arg value 1.07654321E-9
get_class_methods(): Argument #1 ($object_or_class) must be an object or a valid class name, float given

Arg value 0.5
get_class_methods(): Argument #1 ($object_or_class) must be an object or a valid class name, float given
Error: 2 - Array to string conversion

Arg value Array
get_class_methods(): Argument #1 ($object_or_class) must be an object or a valid class name, array given
Error: 2 - Array to string conversion

Arg value Array
get_class_methods(): Argument #1 ($object_or_class) must be an object or a valid class name, array given
Error: 2 - Array to string conversion

Arg value Array
get_class_methods(): Argument #1 ($object_or_class) must be an object or a valid class name, array given
Error: 2 - Array to string conversion

Arg value Array
get_class_methods(): Argument #1 ($object_or_class) must be an object or a valid class name, array given
Error: 2 - Array to string conversion

Arg value Array
get_class_methods(): Argument #1 ($object_or_class) must be an object or a valid class name, array given

Arg value 
get_class_methods(): Argument #1 ($object_or_class) must be an object or a valid class name, null given

Arg value 
get_class_methods(): Argument #1 ($object_or_class) must be an object or a valid class name, null given

Arg value 1
get_class_methods(): Argument #1 ($object_or_class) must be an object or a valid class name, bool given

Arg value 
get_class_methods(): Argument #1 ($object_or_class) must be an object or a valid class name, bool given

Arg value 1
get_class_methods(): Argument #1 ($object_or_class) must be an object or a valid class name, bool given

Arg value 
get_class_methods(): Argument #1 ($object_or_class) must be an object or a valid class name, bool given

Arg value 
get_class_methods(): Argument #1 ($object_or_class) must be an object or a valid class name, string given

Arg value 
get_class_methods(): Argument #1 ($object_or_class) must be an object or a valid class name, string given

Arg value string
get_class_methods(): Argument #1 ($object_or_class) must be an object or a valid class name, string given

Arg value string
get_class_methods(): Argument #1 ($object_or_class) must be an object or a valid class name, string given

Arg value stdClass
array(0) {
}

Arg value 
get_class_methods(): Argument #1 ($object_or_class) must be an object or a valid class name, null given

Arg value 
get_class_methods(): Argument #1 ($object_or_class) must be an object or a valid class name, null given
Done
