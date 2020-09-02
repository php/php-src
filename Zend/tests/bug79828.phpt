--TEST--
Bug #79828: Segfault when trying to access non-existing variable
--FILE--
<?php
function foo(): AnyType {
   return $uninitialized;
}
foo();
?>
--EXPECTF--
Warning: Undefined variable $uninitialized in %s on line %d

Fatal error: Uncaught TypeError: foo(): Return value must be of type AnyType, null returned in %s:%d
Stack trace:
#0 %s(%d): foo()
#1 {main}
  thrown in %s on line %d
