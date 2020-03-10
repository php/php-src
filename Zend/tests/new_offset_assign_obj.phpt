--TEST--
Assigning to a property of a new offset
--FILE--
<?php

$arr[][]->bar = 2;

?>
--EXPECTF--
Fatal error: Uncaught Error: Cannot use [] for reading in %s:%d
Stack trace:
#0 {main}
  thrown in %s on line %d
