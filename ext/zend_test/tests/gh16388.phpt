--TEST--
GH-16388 (UB when freeing a cloned _ZendTestFiber)
--EXTENSIONS--
zend_test
--FILE--
<?php
$fiber = new _ZendTestFiber(function (): int {});
clone $fiber;
?>
--EXPECTF--
Fatal error: Uncaught Error: Trying to clone an uncloneable object of class _ZendTestFiber in %s:%d
%A
