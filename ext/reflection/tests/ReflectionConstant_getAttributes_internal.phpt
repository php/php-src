--TEST--
ReflectionConstant::getAttributes() with attribute (internal constant)
--FILE--
<?php

$reflectionConstant = new ReflectionConstant('E_STRICT');
var_dump($reflectionConstant->getAttributes());

?>
--EXPECTF--
array(1) {
  [0]=>
  object(ReflectionAttribute)#%d (1) {
    ["name"]=>
    string(10) "Deprecated"
  }
}
