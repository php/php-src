--TEST--
__PHP_Incomplete_Class cannot be instantiated
--FILE--
<?php
var_dump(__PHP_Incomplete_Class::class);
var_dump(new __PHP_Incomplete_Class());
?>
--EXPECTF--
string(22) "__PHP_Incomplete_Class"

Fatal error: Uncaught Error: Call to private __PHP_Incomplete_Class::__construct() from global scope in %s:%d
Stack trace:
#0 {main}
  thrown in %s on line %d
