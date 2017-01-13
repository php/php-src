--TEST--
Overloaded function 001
--SKIPIF--
<?php
if (!PHP_DEBUG) die("skip only run in debug version");
?>
--FILE--
<?php
var_dump(_ZendTestClass::test());
?>
--EXPECTF--
Fatal error: Uncaught Error: Cannot call overloaded function for non-object in %soverloaded_func_001.php:%d
Stack trace:
#0 {main}
  thrown in %soverloaded_func_001.php on line %d
