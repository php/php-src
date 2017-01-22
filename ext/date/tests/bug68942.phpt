--TEST--
Bug #68942 (Use after free vulnerability in unserialize() with DateTimeZone).
--FILE--
<?php
$data = unserialize('a:2:{i:0;O:12:"DateTimeZone":2:{s:13:"timezone_type";a:2:{i:0;i:1;i:1;i:2;}s:8:"timezone";s:1:"A";}i:1;R:4;}');
var_dump($data);
?>
--EXPECTF--
Fatal error: Uncaught Error: Timezone initialization failed in %s:%d
Stack trace:
#0 [internal function]: DateTimeZone->__wakeup()
#1 %s(%d): unserialize('a:2:{i:0;O:12:"...')
#2 {main}
  thrown in %s on line %d
