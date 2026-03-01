--TEST--
__PHP_Incomplete_Class cannot be instantiated
--FILE--
<?php
var_dump(__PHP_Incomplete_Class::class);
var_dump(new __PHP_Incomplete_Class());
?>
--EXPECTF--
string(22) "__PHP_Incomplete_Class"

Fatal error: Uncaught Error: Instantiation of class Closure is not allowed in %s:%d
Stack trace:
#0 {main}
  thrown in %s on line %d
