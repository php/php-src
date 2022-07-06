--TEST--
AppendIterator::__construct() with invalid arguments
--FILE--
<?php

new AppendIterator(null);

?>
--EXPECTF--
Fatal error: Uncaught ArgumentCountError: AppendIterator::__construct() expects exactly 0 arguments, 1 given in %s:%d
Stack trace:
#0 %s(%d): AppendIterator->__construct(NULL)
#1 {main}
  thrown in %s on line %d
