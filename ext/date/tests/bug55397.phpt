--TEST--
Bug #55397 (comparison of incomplete DateTime causes SIGSEGV)
--FILE--
<?php
date_default_timezone_set('Europe/Prague');
var_dump(unserialize('O:8:"DateTime":0:{}') == new DateTime);
?>
--EXPECTF--
Fatal error: Uncaught Error: Invalid serialization data for DateTime object in %sbug55397.php:%d
Stack trace:
#0 [internal function]: DateTime->__unserialize(Array)
#1 %sbug55397.php(%d): unserialize('O:8:"DateTime":...')
#2 {main}
  thrown in %sbug55397.php on line %d
