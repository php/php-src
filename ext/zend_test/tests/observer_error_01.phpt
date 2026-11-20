--TEST--
Observer: End handlers fire after a fatal error
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
    var_dump(zend_trigger_bailout());
}

foo();

echo 'You should not see this.';
?>
--EXPECTF--
<!-- init '%s' -->
<file '%s'>
  <!-- init foo() -->
  <foo>
    <!-- init zend_trigger_bailout() -->
    <zend_trigger_bailout>

Fatal error: Bailout in %s on line %d
    </zend_trigger_bailout:NULL>
  </foo:NULL>
</file '%s%eobserver_error_%d.php'>
