--TEST--
Attributes with TARGET_ALL (from the default) can target constants
--FILE--
<?php

#[Attribute]
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
