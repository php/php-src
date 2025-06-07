--TEST--
Parameter is required
--FILE--
<?php

#[ClassAlias]
class Demo {}

?>
--EXPECTF--
Fatal error: Uncaught ArgumentCountError: ClassAlias::__construct() expects at least 1 argument, 0 given in %s:%d
Stack trace:
#0 %s(%d): ClassAlias->__construct()
#1 {main}
  thrown in %s on line %d
