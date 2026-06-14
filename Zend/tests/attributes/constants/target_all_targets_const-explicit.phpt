--TEST--
Attributes with TARGET_ALL (from an explicit parameter) can target constants
--FILE--
<?php

#[Attribute(Attribute::TARGET_ALL)]
class MyAttribute {}

#[MyAttribute]
const EXAMPLE = 'Foo';

$ref = new ReflectionConstant('EXAMPLE');
$attribs = $ref->getAttributes();
var_dump($attribs);
$attribs[0]->newInstance();

?>
--EXPECTF--
array(1) {
  [0]=>
  object(ReflectionAttribute)#%d (1) {
    ["name"]=>
    string(11) "MyAttribute"
  }
}
