--TEST--
Observer: Observe basic TypeError
--EXTENSIONS--
zend_test
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
Warning: iterable type is now a compile time alias for array|Traversable, regenerate the argument info via the php-src gen_stub build script in Unknown on line 0

Warning: iterable type is now a compile time alias for array|Traversable, regenerate the argument info via the php-src gen_stub build script in Unknown on line 0
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
