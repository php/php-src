--TEST--
Test for bug #66721: __unserialize of DateTime segfaults when invalid object data is supplied
--CREDITS--
Boro Sitnikovski <buritomath@yahoo.com>
--FILE--
<?php
$y = 'O:8:"DateTime":3:{s:4:"date";s:19:"2014-02-15 02:00:51";s:13:"timezone_type";i:3;s:8:"timezone";s:10:"1234567890";}';
var_dump(unserialize($y));
?>
--EXPECTF--
Fatal error: Uncaught Error: Invalid serialization data for DateTime object in %sbug66721.php:%d
Stack trace:
#0 [internal function]: DateTime->__unserialize(Array)
#1 %sbug66721.php(%d): unserialize('O:8:"DateTime":...')
#2 {main}
  thrown in %sbug66721.php on line %d
