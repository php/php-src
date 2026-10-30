--TEST--
Test array_product() function : variation
--FILE--
<?php
echo "*** Testing array_product() : variation - using non numeric values ***\n";

class A {
  static function help() { echo "hello\n"; }
}

$types = array("boolean (true)" => true, "boolean (false)" => false,
       "string" => "hello", "numeric string" =>  "12",
       "resource" => STDERR, "object" => new A(), "null" => null,
       "array" => array(3,2));

foreach ($types as $desc => $type) {
  echo $desc, "\n";
  var_dump(array_product([1, $type]));
  echo "\n";
}

?>
--EXPECTF--
*** Testing array_product() : variation - using non numeric values ***
boolean (true)
int(1)

boolean (false)
int(0)

string

Warning: array_product(): Multiplication is not supported on type string in %s on line %d
int(0)

numeric string
int(12)

resource

Warning: array_product(): Multiplication is not supported on type resource in %s on line %d
int(3)

object

Warning: array_product(): Multiplication is not supported on type A in %s on line %d
int(1)

null
int(0)

array

Warning: array_product(): Multiplication is not supported on type array in %s on line %d
int(1)

