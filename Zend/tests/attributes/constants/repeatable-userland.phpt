--TEST--
Validation of attribute repetition (is allowed; userland attribute)
--FILE--
<?php

#[Attribute(Attribute::TARGET_ALL|Attribute::IS_REPEATABLE)]
class MyAttribute {}

#[MyAttribute]
#[MyAttribute]
const MY_CONST = true;

$attributes = new ReflectionConstant('MY_CONST')->getAttributes();
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
