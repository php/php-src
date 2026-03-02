--TEST--
GH-11580: assert() with ??= operator can lead to use-of-uninitialized-value
--INI--
zend.assertions=0
--FILE--
<?php
assert(y)[y] ??= y;
?>
--EXPECTF--
Fatal error: Uncaught Error: Undefined constant "y" in %s:%d
Stack trace:
#0 {main}
  thrown in %s on line %d
