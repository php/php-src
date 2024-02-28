--TEST--
Observer: End handlers fire after a userland fatal error
--EXTENSIONS--
zend_test
--INI--
zend_test.observer.enabled=1
zend_test.observer.observe_all=1
zend_test.observer.show_return_value=1
--FILE--
<?php
set_error_handler(function ($errno, $errstr, $errfile, $errline) {
    trigger_error('Foo error', E_USER_ERROR);
});

function foo()
{
	return $x; // warning
}

foo();

echo 'You should not see this.';
?>
--EXPECTF--
<!-- init '%s' -->
<file '%s'>
  <!-- init set_error_handler() -->
  <set_error_handler>
  </set_error_handler:NULL>
  <!-- init foo() -->
  <foo>
    <!-- init {closure:%s:%d}() -->
    <{closure:%s:%d}>
      <!-- init trigger_error() -->
      <trigger_error>

Fatal error: Foo error in %s on line %d
      </trigger_error:NULL>
    </{closure:%s:%d}:NULL>
  </foo:NULL>
</file '%s'>
