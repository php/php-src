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

Fatal error: Uncaught Error: Attribute "MyAttribute" must not be repeated in %s:%d
Stack trace:
#0 %s(%d): ReflectionAttribute->newInstance()
#1 {main}
  thrown in %s on line %d
