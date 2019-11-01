--TEST--
Test usleep() function : error conditions
--FILE--
<?php

usleep(-10);

?>
--EXPECTF--
Fatal error: Uncaught ValueError: Number of microseconds must be greater than or equal to 0 in %s
Stack trace:
#0 %s(%d): usleep(-10)
#1 {main}
  thrown in %s on line %d
