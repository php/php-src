--TEST--
Bug #81430 (Attribute instantiation leaves dangling execute_data pointer)
--EXTENSIONS--
zend_test
--INI--
memory_limit=20M
zend_test.observer.enabled=1
zend_test.observer.observe_all=1
--SKIPIF--
<?php
if (getenv("USE_ZEND_ALLOC") === "0") die("skip requires zmm");
?>
--FILE--
<?php

#[\Attribute]
class A {
    public function __construct() {
        array_map("str_repeat", ["\xFF"], [100000000]); // cause a bailout
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
      <!-- init array_map() -->
      <array_map>
        <!-- init str_repeat() -->
        <str_repeat>

Fatal error: Allowed memory size of %d bytes exhausted %s in %s on line %d
        </str_repeat>
      </array_map>
    </A::__construct>
  </ReflectionAttribute::newInstance>
</file '%s'>
