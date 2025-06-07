--TEST--
Attributes with TARGET_ALL (from an explicit parameter) can target class aliases
--FILE--
<?php

#[Attribute(Attribute::TARGET_ALL)]
class MyAttribute {}

#[ClassAlias('Other', [new MyAttribute()])]
class Demo {}

$ref = new ReflectionClassAlias('Other');
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
