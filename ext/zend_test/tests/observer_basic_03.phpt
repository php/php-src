--TEST--
Observer: Basic observability of includes
--SKIPIF--
<?php if (!extension_loaded('zend-test')) die('skip: zend-test extension required'); ?>
--INI--
zend_test.observer.enabled=1
zend_test.observer.observe_all=1
--FILE--
<?php
function foo()
{
    echo 'Foo' . PHP_EOL;
}

foo();
include __DIR__ . '/observer.inc';
foo();
?>
--EXPECTF--
<!-- init '%s/observer_basic_03.php' -->
<file '%s/observer_basic_03.php'>
  <!-- init foo() -->
  <foo>
Foo
  </foo>
  <!-- init '%s/observer.inc' -->
  <file '%s/observer.inc'>
    <!-- init foo_observer_test() -->
    <foo_observer_test>
foo_observer_test
    </foo_observer_test>
  </file '%s/observer.inc'>
  <foo>
Foo
  </foo>
</file '%s/observer_basic_03.php'>
