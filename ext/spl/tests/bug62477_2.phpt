--TEST--
Bug #62477 LimitIterator int overflow when float is passed (2)
--FILE--
<?php

$it = new LimitIterator(new ArrayIterator(array(42)), 0, 10000000000000000000);
--EXPECTF--
Fatal error: Uncaught exception 'OutOfRangeException' with message 'count param must be of type int' in %sbug62477_2.php:%d
Stack trace:
#0 %sbug62477_2.php(%d): LimitIterator->__construct(Object(ArrayIterator), 0, %f)
#1 {main}
  thrown in %sbug62477_2.php on line %d
