--TEST--
Bug #80030: Optimizer segfault with isset on static property with undef dynamic class name
--FILE--
<?php

function test() {
    var_dump(isset($className::$test));
}
test();

?>
--EXPECTF--
Warning: Undefined variable $className in %s on line %d

Fatal error: Uncaught Error: Class name must be a valid object or a string in %s:%d
Stack trace:
#0 %s(%d): test()
#1 {main}
  thrown in %s on line %d
