--TEST--
Test sleep() function : error conditions
--FILE--
<?php

sleep(-10);

?>
--EXPECTF--
Fatal error: Uncaught ValueError: sleep(): Argument #1 ($seconds) must be between 0 and %d in %s:%d
Stack trace:
#0 %s(%d): sleep(-10)
#1 {main}
  thrown in %s on line %d
