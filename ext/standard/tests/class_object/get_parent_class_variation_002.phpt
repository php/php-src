--TEST--
Test get_parent_class() function : usage variations  - unexpected argument type.
--FILE--
<?php
spl_autoload_register(function ($className) {
    echo "In autoload($className)\n";
});

function test_error_handler($err_no, $err_msg, $filename, $linenum) {
    echo "Error: $err_no - $err_msg\n";
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

foreach ($values as $value) {
    echo "\nArg value " . (is_object($value) ? get_class($value) : $value) . "\n";
    try {
        var_dump(get_parent_class($value));
    } catch (TypeError $exception) {
        echo $exception->getMessage() . "\n";
    }
}

echo "Done";
?>
--EXPECT--
*** Testing get_parent_class() : usage variations ***
Error: 2 - Undefined variable $undefined_var
Error: 2 - Undefined variable $unset_var

Arg value 0
get_parent_class(): Argument #1 ($object_or_class) must be an object or a valid class name, int given

Arg value 1
get_parent_class(): Argument #1 ($object_or_class) must be an object or a valid class name, int given

Arg value 12345
get_parent_class(): Argument #1 ($object_or_class) must be an object or a valid class name, int given

Arg value -2345
get_parent_class(): Argument #1 ($object_or_class) must be an object or a valid class name, int given

Arg value 10.5
get_parent_class(): Argument #1 ($object_or_class) must be an object or a valid class name, float given

Arg value -10.5
get_parent_class(): Argument #1 ($object_or_class) must be an object or a valid class name, float given

Arg value 101234567000
get_parent_class(): Argument #1 ($object_or_class) must be an object or a valid class name, float given

Arg value 1.07654321E-9
get_parent_class(): Argument #1 ($object_or_class) must be an object or a valid class name, float given

Arg value 0.5
get_parent_class(): Argument #1 ($object_or_class) must be an object or a valid class name, float given
Error: 2 - Array to string conversion

Arg value Array
get_parent_class(): Argument #1 ($object_or_class) must be an object or a valid class name, array given
Error: 2 - Array to string conversion

Arg value Array
get_parent_class(): Argument #1 ($object_or_class) must be an object or a valid class name, array given
Error: 2 - Array to string conversion

Arg value Array
get_parent_class(): Argument #1 ($object_or_class) must be an object or a valid class name, array given
Error: 2 - Array to string conversion

Arg value Array
get_parent_class(): Argument #1 ($object_or_class) must be an object or a valid class name, array given
Error: 2 - Array to string conversion

Arg value Array
get_parent_class(): Argument #1 ($object_or_class) must be an object or a valid class name, array given

Arg value 
get_parent_class(): Argument #1 ($object_or_class) must be an object or a valid class name, null given

Arg value 
get_parent_class(): Argument #1 ($object_or_class) must be an object or a valid class name, null given

Arg value 1
get_parent_class(): Argument #1 ($object_or_class) must be an object or a valid class name, true given

Arg value 
get_parent_class(): Argument #1 ($object_or_class) must be an object or a valid class name, false given

Arg value 1
get_parent_class(): Argument #1 ($object_or_class) must be an object or a valid class name, true given

Arg value 
get_parent_class(): Argument #1 ($object_or_class) must be an object or a valid class name, false given

Arg value 
get_parent_class(): Argument #1 ($object_or_class) must be an object or a valid class name, string given

Arg value 
get_parent_class(): Argument #1 ($object_or_class) must be an object or a valid class name, string given

Arg value string
In autoload(string)
get_parent_class(): Argument #1 ($object_or_class) must be an object or a valid class name, string given

Arg value String
In autoload(String)
get_parent_class(): Argument #1 ($object_or_class) must be an object or a valid class name, string given

Arg value stdClass
bool(false)

Arg value 
get_parent_class(): Argument #1 ($object_or_class) must be an object or a valid class name, null given

Arg value 
get_parent_class(): Argument #1 ($object_or_class) must be an object or a valid class name, null given
Done
