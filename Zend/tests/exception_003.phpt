--TEST--
Throwing exception in global scope
--FILE--
<?php 

throw new Exception(1);

?>
--EXPECTF--

Exception: 1 in %s on line %d
Stack trace:
#0 {main}
