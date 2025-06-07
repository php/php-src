--TEST--
Validation of attribute repetition (not allowed; userland attribute)
--FILE--
<?php

#[Attribute]
class MyAttribute {}

#[ClassAlias('Other', [new MyAttribute(), new MyAttribute()])]
class Demo {}

$attributes = new ReflectionClassAlias('Other')->getAttributes();
var_dump($attributes);
$attributes[0]->newInstance();

?>
--EXPECTF--
array(1) {
  [0]=>
  object(ReflectionAttribute)#%d (1) {
    ["name"]=>
    string(10) "ClassAlias"
  }
}
