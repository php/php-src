--TEST--
Bug #62477 LimitIterator int overflow when float is passed (1)
--FILE--
<?php

$it = new LimitIterator(new ArrayIterator(array(42)), 10000000000000000000);
--EXPECTF--
Fatal error: Uncaught exception 'OutOfRangeException' with message 'offset param must be of type int' in %sbug62477_1.php:%d
Stack trace:
#0 %sbug62477_1.php(%d): LimitIterator->__construct(Object(ArrayIterator), %f)
#1 {main}
  thrown in %sbug62477_1.php on line %d
