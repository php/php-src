--TEST--
#[\Deprecated]: Works in stubs.
--EXTENSIONS--
zend_test
--FILE--
<?php
zend_test_deprecated();
zend_test_deprecated_attr();

$reflection = new ReflectionFunction('zend_test_deprecated_attr');
var_dump($reflection->getAttributes()[0]->newInstance());
var_dump($reflection->isDeprecated());

_ZendTestClass::ZEND_TEST_DEPRECATED_ATTR;

$reflection = new ReflectionClassConstant('_ZendTestClass', 'ZEND_TEST_DEPRECATED_ATTR');
var_dump($reflection->getAttributes()[0]->newInstance());
var_dump($reflection->isDeprecated());

?>
--EXPECTF--
Deprecated: Function zend_test_deprecated() is deprecated in %s on line %d

Deprecated: Function zend_test_deprecated_attr() is deprecated, custom message in %s on line %d
object(Deprecated)#%d (2) {
  ["message"]=>
  string(14) "custom message"
  ["since"]=>
  NULL
}
bool(true)

Deprecated: Constant _ZendTestClass::ZEND_TEST_DEPRECATED_ATTR is deprecated, custom message in %s on line %d
object(Deprecated)#%d (2) {
  ["message"]=>
  string(14) "custom message"
  ["since"]=>
  NULL
}
bool(true)
