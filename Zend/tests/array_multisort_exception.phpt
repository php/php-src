--TEST--
Exception handling in array_multisort()
--FILE--
<?php
$array = [1 => new DateTime(), 0 => new DateTime()];
array_multisort($array, SORT_STRING);
?>
--EXPECTF--
Fatal error: Uncaught Error: Object of class DateTime could not be converted to string in %s:%d
Stack trace:
#0 %s(%d): array_multisort(Array, 2)
#1 {main}
  thrown in %s on line %d
