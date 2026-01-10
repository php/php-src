--TEST--
Verify that attributes for internal functions correctly support named arguments.
--EXTENSIONS--
zend_test
--FILE--
<?php

$reflection = new ReflectionFunction("zend_test_attribute_with_named_argument");
$attribute = $reflection->getAttributes()[0];
var_dump($attribute->getArguments());
var_dump($attribute->newInstance());

?>
--EXPECTF--
array(1) {
  ["arg"]=>
  string(3) "foo"
}
object(ZendTestAttributeWithArguments)#3 (1) {
  ["arg"]=>
  string(3) "foo"
}
