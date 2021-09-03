--TEST--
Trailing comma in attribute argument list
--FILE--
<?php

#[MyAttribute(
    "there",
    "are",
    "many",
    "arguments",
)]
class Foo { }

$ref = new \ReflectionClass(Foo::class);
$attr = $ref->getAttributes()[0];
var_dump($attr->getName(), $attr->getArguments());

?>
--EXPECT--
string(11) "MyAttribute"
array(4) {
  [0]=>
  string(5) "there"
  [1]=>
  string(3) "are"
  [2]=>
  string(4) "many"
  [3]=>
  string(9) "arguments"
}
