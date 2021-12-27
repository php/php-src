--TEST--
Bug #81430 (Attribute instantiation frame has no run time cache)
--INI--
memory_limit=20M
zend_test.observer.enabled=1
zend_test.observer.observe_all=1
--FILE--
<?php

#[\Attribute]
class A {
        public function __construct() {}
}

#[A]
function B() {}

$r = new \ReflectionFunction("B");
call_user_func([$r->getAttributes(A::class)[0], 'newInstance']);
?>
--EXPECTF--
<!-- init '%s' -->
<file '%s'>
  <!-- init A::__construct() -->
  <A::__construct>
  </A::__construct>
</file '%s'>
