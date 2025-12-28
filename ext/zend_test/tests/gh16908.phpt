--TEST--
GH-16908 (_ZendTestMagicCallForward does not handle references well)
--EXTENSIONS--
zend_test
--FILE--
<?php
$cls = new _ZendTestMagicCallForward();
function &foo() {
}
$cls->foo()->x ??= 42;
?>
--EXPECTF--
Notice: Only variable references should be returned by reference in %s on line %d

Fatal error: Uncaught Error: Attempt to assign property "x" on null in %s:%d
Stack trace:
#0 {main}
  thrown in %s on line %d
