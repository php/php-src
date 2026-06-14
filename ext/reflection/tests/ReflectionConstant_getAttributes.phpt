--TEST--
ReflectionConstant::getAttributes() with attribute
--FILE--
<?php

#[Foo]
const CT_CONST = 42;
$reflectionConstant = new ReflectionConstant('CT_CONST');
var_dump($reflectionConstant->getAttributes());

?>
--EXPECTF--
array(1) {
  [0]=>
  object(ReflectionAttribute)#%d (1) {
    ["name"]=>
    string(3) "Foo"
  }
}
