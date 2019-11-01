--TEST--
Test sleep() function : error conditions
--FILE--
<?php

sleep(-10);

?>
--EXPECTF--
Fatal error: Uncaught ValueError: Number of seconds must be greater than or equal to 0 in %s
Stack trace:
#0 %s(%d): sleep(-10)
#1 {main}
  thrown in %s on line %d
