--TEST--
Throwing exception in global scope
--FILE--
<?php 

throw new Exception(1);

?>
--EXPECTF--

Fatal error: Uncaught exception 'Exception' with message '1' in %s:%d
Stack trace:
#0 {main}
  thrown in %s on line %d
