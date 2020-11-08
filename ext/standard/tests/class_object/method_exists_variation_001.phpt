--TEST--
Test method_exists() function : usage variations  - unexpected type for arg 1
--FILE--
<?php
spl_autoload_register(function ($className) {
    echo "In autoload($className)\n";
});

function test_error_handler($err_no, $err_msg, $filename, $linenum) {
    echo "Error: $err_no - $err_msg\n";
}
set_error_handler('test_error_handler');

echo "*** Testing method_exists() : usage variations ***\n";

// Initialise function arguments not being substituted (if any)
$method = 'string_val';

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
      echo "\nArg value $value \n";
      try {
        var_dump( method_exists($value, $method) );
      } catch (TypeError $e) {
        echo $e->getMessage(), PHP_EOL;
      }
};

echo "Done";
?>
--EXPECT--
*** Testing method_exists() : usage variations ***
Error: 2 - Undefined variable $undefined_var
Error: 2 - Undefined variable $unset_var

Arg value 0 
method_exists(): Argument #1 ($object_or_class) must be of type object|string, int given

Arg value 1 
method_exists(): Argument #1 ($object_or_class) must be of type object|string, int given

Arg value 12345 
method_exists(): Argument #1 ($object_or_class) must be of type object|string, int given

Arg value -2345 
method_exists(): Argument #1 ($object_or_class) must be of type object|string, int given

Arg value 10.5 
method_exists(): Argument #1 ($object_or_class) must be of type object|string, float given

Arg value -10.5 
method_exists(): Argument #1 ($object_or_class) must be of type object|string, float given

Arg value 101234567000 
method_exists(): Argument #1 ($object_or_class) must be of type object|string, float given

Arg value 1.07654321E-9 
method_exists(): Argument #1 ($object_or_class) must be of type object|string, float given

Arg value 0.5 
method_exists(): Argument #1 ($object_or_class) must be of type object|string, float given
Error: 2 - Array to string conversion

Arg value Array 
method_exists(): Argument #1 ($object_or_class) must be of type object|string, array given
Error: 2 - Array to string conversion

Arg value Array 
method_exists(): Argument #1 ($object_or_class) must be of type object|string, array given
Error: 2 - Array to string conversion

Arg value Array 
method_exists(): Argument #1 ($object_or_class) must be of type object|string, array given
Error: 2 - Array to string conversion

Arg value Array 
method_exists(): Argument #1 ($object_or_class) must be of type object|string, array given
Error: 2 - Array to string conversion

Arg value Array 
method_exists(): Argument #1 ($object_or_class) must be of type object|string, array given

Arg value  
method_exists(): Argument #1 ($object_or_class) must be of type object|string, null given

Arg value  
method_exists(): Argument #1 ($object_or_class) must be of type object|string, null given

Arg value 1 
method_exists(): Argument #1 ($object_or_class) must be of type object|string, bool given

Arg value  
method_exists(): Argument #1 ($object_or_class) must be of type object|string, bool given

Arg value 1 
method_exists(): Argument #1 ($object_or_class) must be of type object|string, bool given

Arg value  
method_exists(): Argument #1 ($object_or_class) must be of type object|string, bool given

Arg value  
bool(false)

Arg value  
bool(false)

Arg value string 
In autoload(string)
bool(false)

Arg value String 
In autoload(String)
bool(false)

Arg value  
method_exists(): Argument #1 ($object_or_class) must be of type object|string, null given

Arg value  
method_exists(): Argument #1 ($object_or_class) must be of type object|string, null given
Done
