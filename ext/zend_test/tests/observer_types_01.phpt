--TEST--
Observer: Observe basic TypeError
--EXTENSIONS--
zend_test
--INI--
zend_test.observer.enabled=1
zend_test.observer.show_output=1
zend_test.observer.observe_all=1
zend_test.observer.show_return_value=1
--FILE--
<?php
function foo(array $a) { return 1; }
foo(42);
?>
--EXPECTF--
<!-- init '%s' -->
<file '%s'>
  <!-- init foo() -->
  <foo>
    <!-- Exception: TypeError -->
  </foo:NULL>
  <!-- Exception: TypeError -->
</file '%s'>
<!-- init Error::__toString() -->
<Error::__toString>
  <!-- init Error::getTraceAsString() -->
  <Error::getTraceAsString>
  </Error::getTraceAsString:'#0 %s(%d): foo(42)
#1 {main}'>
</Error::__toString:'TypeError: foo(): Argument #1 ($a) must be of type array, int given, called in %s:%d
Stack trace:
#0 %s%eobserver_types_%d.php(%d): foo(42)
#1 {main}'>

Fatal error: Uncaught TypeError: foo(): Argument #1 ($a) must be of type array, int given, called in %s:%d
Stack trace:
#0 %s%eobserver_types_%d.php(%d): foo(42)
#1 {main}
  thrown in %s%eobserver_types_%d.php on line %d
