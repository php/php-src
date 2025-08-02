--TEST--
Test observing inherited internal functions
--EXTENSIONS--
zend_test
--INI--
opcache.enable_cli=1
zend_test.observer.enabled=1
zend_test.observer.show_output=1
zend_test.observer.observe_all=1
zend_test.observer.show_return_value=1
--FILE--
<?php

class MyReflectionProperty extends ReflectionProperty {
}

class A {
    protected $protected = 'a';
}

$property = new MyReflectionProperty('A', 'protected');
$property->isStatic();

?>
--EXPECTF--
<!-- init '%s' -->
<file '%s'>
  <!-- init ReflectionProperty::__construct() -->
  <ReflectionProperty::__construct>
  </ReflectionProperty::__construct:NULL>
  <!-- init ReflectionProperty::isStatic() -->
  <ReflectionProperty::isStatic>
  </ReflectionProperty::isStatic:false>
</file '%s'>
