--TEST--
Multiple attributes in separate groups are allowed
--FILE--
<?php

#[\Foo]
#[\Bar]
const CONSTANT = 1;

$ref = new ReflectionConstant('CONSTANT');
var_dump($ref->getAttributes());

?>
--EXPECTF--
array(2) {
  [0]=>
  object(ReflectionAttribute)#%d (1) {
    ["name"]=>
    string(3) "Foo"
  }
  [1]=>
  object(ReflectionAttribute)#%d (1) {
    ["name"]=>
    string(3) "Bar"
  }
}
