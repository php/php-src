--TEST--
Observer: Retvals are observable that are: refcounted, IS_CV
--SKIPIF--
<?php if (!extension_loaded('zend-test')) die('skip: zend-test extension required'); ?>
--INI--
zend_test.observer.enabled=1
zend_test.observer.observe_all=1
zend_test.observer.show_return_value=1
--FILE--
<?php
class MyRetval {}

function foo() {
    $retval = new MyRetval(); // Refcounted
    return $retval; // IS_CV
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
  </foo:object(MyRetval)#%d>
  <foo>
  </foo:object(MyRetval)#%d>
Done
</file '%s/observer_retval_%d.php'>
