--TEST--
Test array_flip() function : usage variations - 'input' argument with different invalid values for keys
--FILE--
<?php
/*
* Trying different invalid values for 'input' array argument
*/

echo "*** Testing array_flip() : different invalid values in 'input' array argument ***\n";

// class definition for object data
class MyClass
{
   public function __toString()
   {
     return 'object';
   }
}
$obj = new MyClass();

// resource data
$fp = fopen(__FILE__, 'r');

$input = array(
  // float values
  'float_value1' => 1.2,
  'float_value2' => 0.5,
  'flaot_value3' => 3.4E3,
  'flaot_value4' => 5.6E-6,

  // bool values
  'bool_value1' => true,
  'bool_value2' => false,
  'bool_value3' => TRUE,
  'bool_value4' => FALSE,

  // null values
  'null_value1' => null,

  // array value
  'array_value' => array(1),

  // object value
  'obj_value' => $obj,

  // resource value
  'resource_value' => $fp,
);

var_dump( array_flip($input) );

// closing resource
fclose($fp);

echo "Done"
?>
--EXPECTF--
*** Testing array_flip() : different invalid values in 'input' array argument ***

Warning: array_flip(): Can only flip string and integer values, entry skipped in %s on line %d

Warning: array_flip(): Can only flip string and integer values, entry skipped in %s on line %d

Warning: array_flip(): Can only flip string and integer values, entry skipped in %s on line %d

Warning: array_flip(): Can only flip string and integer values, entry skipped in %s on line %d

Warning: array_flip(): Can only flip string and integer values, entry skipped in %s on line %d

Warning: array_flip(): Can only flip string and integer values, entry skipped in %s on line %d

Warning: array_flip(): Can only flip string and integer values, entry skipped in %s on line %d

Warning: array_flip(): Can only flip string and integer values, entry skipped in %s on line %d

Warning: array_flip(): Can only flip string and integer values, entry skipped in %s on line %d

Warning: array_flip(): Can only flip string and integer values, entry skipped in %s on line %d

Warning: array_flip(): Can only flip string and integer values, entry skipped in %s on line %d

Warning: array_flip(): Can only flip string and integer values, entry skipped in %s on line %d
array(0) {
}
Done
