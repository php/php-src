--TEST--
tests Fiber::yield out of fiber internal call
--FILE--
<?php
Fiber::yield();
--EXPECTF--
Fatal error: Uncaught Error: Cannot call Fiber::yield out of Fiber in %s/Zend/tests/fiber/009.php:2
Stack trace:
#0 %s/Zend/tests/fiber/009.php(2): Fiber::yield()
#1 {main}
  thrown in %s/Zend/tests/fiber/009.php on line 2
