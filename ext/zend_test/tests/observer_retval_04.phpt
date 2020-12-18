--TEST--
Observer: Retvals are observable that are: refcounted, IS_VAR
--SKIPIF--
<?php if (!extension_loaded('zend-test')) die('skip: zend-test extension required'); ?>
--INI--
zend_test.observer.enabled=1
zend_test.observer.observe_all=1
zend_test.observer.show_return_value=1
--FILE--
<?php
class MyRetval {}

function getObj() {
    return new MyRetval(); // Refcounted
}

function foo() {
    return getObj(); // IS_VAR
}

$res = foo(); // Retval used
foo(); // Retval unused

function bar($what) {
  return 'This gets ' . $what . ' in the return handler when unused'; // Refcounted + IS_VAR
}

$res = bar('freed'); // Retval used
bar('freed'); // Retval unused

echo 'Done' . PHP_EOL;
?>
--EXPECTF--
<!-- init '%s/observer_retval_%d.php' -->
<file '%s/observer_retval_%d.php'>
  <!-- init foo() -->
  <foo>
    <!-- init getObj() -->
    <getObj>
    </getObj:object(MyRetval)#%d>
  </foo:object(MyRetval)#%d>
  <foo>
    <getObj>
    </getObj:object(MyRetval)#%d>
  </foo:object(MyRetval)#%d>
  <!-- init bar() -->
  <bar>
  </bar:'This gets freed in the return handler when unused'>
  <bar>
  </bar:'This gets freed in the return handler when unused'>
Done
</file '%s/observer_retval_%d.php'>
