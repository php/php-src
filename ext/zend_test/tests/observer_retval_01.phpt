--TEST--
Observer: Retvals are observable that are: IS_CONST
--SKIPIF--
<?php if (!extension_loaded('zend-test')) die('skip: zend-test extension required'); ?>
--INI--
zend_test.observer.enabled=1
zend_test.observer.observe_all=1
zend_test.observer.show_return_value=1
--FILE--
<?php
function foo() {
    return 'I should be observable'; // IS_CONST
}

$res = foo(); // Retval used
foo(); // Retval unused

echo 'Done' . PHP_EOL;
?>
--EXPECTF--
<!-- init '%s/observer_retval_%d.php' -->
<file '%s/observer_retval_%d.php'>
  <!-- init foo() -->
  <foo>
  </foo:'I should be observable'>
  <foo>
  </foo:'I should be observable'>
Done
</file '%s/observer_retval_%d.php'>
