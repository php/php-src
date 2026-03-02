--TEST--
gen_stub.php: Test that attributes are applied to constants, properties, and methods
--EXTENSIONS--
zend_test
--FILE--
<?php

$reflectionConstant = new ReflectionClassConstant(ZendAttributeTest::class, "TEST_CONST");
var_dump($reflectionConstant->getAttributes()[0]->newInstance());
var_dump($reflectionConstant->getAttributes()[1]->newInstance());

$reflectionProperty = new ReflectionProperty(ZendAttributeTest::class, "testProp");
var_dump($reflectionProperty->getAttributes()[0]->newInstance());
var_dump($reflectionProperty->getAttributes()[1]->newInstance());

$reflectionMethod = new ReflectionMethod(ZendAttributeTest::class, "testMethod");
var_dump($reflectionMethod->getAttributes()[0]->newInstance());

?>
--EXPECTF--
object(ZendTestRepeatableAttribute)#%d (%d) {
}
object(ZendTestRepeatableAttribute)#%d (%d) {
}
object(ZendTestRepeatableAttribute)#%d (%d) {
}
object(ZendTestPropertyAttribute)#%d (%d) {
  ["parameter"]=>
  string(%d) "testProp"
}
object(ZendTestAttribute)#%d (%d) {
}
