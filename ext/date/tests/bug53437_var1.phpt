--TEST--
Bug #53437 (Crash when using unserialized DatePeriod instance), variation 2
--FILE--
<?php
$s = 'O:10:"DatePeriod":0:{}';

$dp = unserialize($s);

var_dump($dp);
?>
==DONE==
--EXPECTF--
Fatal error: Uncaught Error: Invalid serialization data for DatePeriod object in %sbug53437_var1.php:%d
Stack trace:
#0 [internal function]: DatePeriod->__wakeup()
#1 %sbug53437_var1.php(%d): unserialize('O:10:"DatePerio...')
#2 {main}
  thrown in %sbug53437_var1.php on line %d
