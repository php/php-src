--TEST--
Observer: End handlers fire after a userland fatal error
--EXTENSIONS--
zend_test
--INI--
zend_test.observer.enabled=1
zend_test.observer.show_output=1
zend_test.observer.observe_all=1
zend_test.observer.show_return_value=1
--FILE--
<?php
function foo()
{
    trigger_error('Foo error', E_USER_ERROR);
}

foo();

echo 'You should not see this.';
?>
--EXPECTF--
<!-- init '%s' -->
<file '%s'>
  <!-- init foo() -->
  <foo>
    <!-- init trigger_error() -->
    <trigger_error>

Fatal error: Foo error in %s on line %d
    </trigger_error:NULL>
  </foo:NULL>
</file '%s'>
