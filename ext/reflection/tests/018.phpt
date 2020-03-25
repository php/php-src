--TEST--
Reflection::getModifierNames
--FILE--
<?php
var_dump(Reflection::getModifierNames(ReflectionMethod::IS_FINAL | ReflectionMethod::IS_PROTECTED));
?>
--EXPECT--
array(2) {
  [0]=>
  string(5) "final"
  [1]=>
  string(9) "protected"
}
