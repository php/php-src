--TEST--
Verify that named parameters can be passed to attributes on constants
--FILE--
<?php

#[MyAttribute(foo: "bar")]
const EXAMPLE = 'Foo';

$ref = new ReflectionConstant('EXAMPLE');
$attribs = $ref->getAttributes();
var_dump($attribs);
var_dump($attribs[0]->getArguments());

?>
--EXPECTF--
array(1) {
  [0]=>
  object(ReflectionAttribute)#%d (1) {
    ["name"]=>
    string(11) "MyAttribute"
  }
}
array(1) {
  ["foo"]=>
  string(3) "bar"
}
