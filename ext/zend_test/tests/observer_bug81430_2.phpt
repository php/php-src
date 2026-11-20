--TEST--
Bug #81430 (Attribute instantiation leaves dangling execute_data pointer)
--EXTENSIONS--
zend_test
--INI--
zend_test.observer.enabled=1
zend_test.observer.show_output=1
zend_test.observer.observe_all=1
--FILE--
<?php

#[\Attribute]
class A {
    public function __construct() {
        var_dump(zend_trigger_bailout());
    }
}

#[A]
function B() {}

$r = new \ReflectionFunction("B");
call_user_func([$r->getAttributes(A::class)[0], 'newInstance']);
?>
--EXPECTF--
<!-- init '%s' -->
<file '%s'>
  <!-- init ReflectionFunction::__construct() -->
  <ReflectionFunction::__construct>
  </ReflectionFunction::__construct>
  <!-- init ReflectionFunctionAbstract::getAttributes() -->
  <ReflectionFunctionAbstract::getAttributes>
  </ReflectionFunctionAbstract::getAttributes>
  <!-- init ReflectionAttribute::newInstance() -->
  <ReflectionAttribute::newInstance>
    <!-- init A::__construct() -->
    <A::__construct>
      <!-- init zend_trigger_bailout() -->
      <zend_trigger_bailout>

Fatal error: Bailout in %s on line %d
      </zend_trigger_bailout>
    </A::__construct>
  </ReflectionAttribute::newInstance>
</file '%s'>
