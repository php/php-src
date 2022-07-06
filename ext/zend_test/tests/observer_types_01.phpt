--TEST--
Observer: Observe basic TypeError
--SKIPIF--
<?php if (!extension_loaded('zend-test')) die('skip: zend-test extension required'); ?>
--INI--
zend_test.observer.enabled=1
zend_test.observer.observe_all=1
zend_test.observer.show_return_value=1
--FILE--
<?php
function foo(array $a) { return 1; }
foo(42);
?>
--EXPECTF--
<!-- init '%s%eobserver_types_%d.php' -->
<file '%s%eobserver_types_%d.php'>
  <!-- init foo() -->
  <foo>
    <!-- Exception: TypeError -->
  </foo:NULL>
  <!-- Exception: TypeError -->
</file '%s%eobserver_types_%d.php'>

Fatal error: Uncaught TypeError: foo(): Argument #1 ($a) must be of type array, int given, called in %s:%d
Stack trace:
#0 %s%eobserver_types_%d.php(%d): foo(42)
#1 {main}
  thrown in %s%eobserver_types_%d.php on line %d
