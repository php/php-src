--TEST--
Validation of attribute repetition (is allowed; userland attribute)
--FILE--
<?php

#[Attribute(Attribute::TARGET_ALL|Attribute::IS_REPEATABLE)]
class MyAttribute {}

#[ClassAlias('Other', [new MyAttribute(), new MyAttribute()])]
class Demo {}

$attributes = new ReflectionClassAlias('Other')->getAttributes();
var_dump($attributes);
$attributes[0]->newInstance();

?>
--EXPECTF--
array(2) {
  [0]=>
  object(ReflectionAttribute)#%d (1) {
    ["name"]=>
    string(11) "MyAttribute"
  }
  [1]=>
  object(ReflectionAttribute)#%d (1) {
    ["name"]=>
    string(11) "MyAttribute"
  }
}
