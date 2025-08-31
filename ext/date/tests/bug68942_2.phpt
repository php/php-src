--TEST--
Bug #68942 (Use after free vulnerability in unserialize() with DateTime).
--FILE--
<?php
$data = unserialize('a:2:{i:0;O:8:"DateTime":3:{s:4:"date";s:26:"2000-01-01 00:00:00.000000";s:13:"timezone_type";a:2:{i:0;i:1;i:1;i:2;}s:8:"timezone";s:1:"A";}i:1;R:5;}');
var_dump($data);
?>
--EXPECTF--
Fatal error: Uncaught Error: Invalid serialization data for DateTime object in %sbug68942_2.php:%d
Stack trace:
#0 [internal function]: DateTime->__unserialize(Array)
#1 %sbug68942_2.php(%d): unserialize('a:2:{i:0;O:8:"D...')
#2 {main}
  thrown in %sbug68942_2.php on line %d
