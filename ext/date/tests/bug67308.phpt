--TEST--
Bug #67308 (Serialize of DateTime truncates fractions of second)
--INI--
date.timezone=America/Vancouver
--FILE--
<?php
// Ensure we can still unserialize the old style.
var_dump(unserialize('O:8:"DateTime":3:{s:4:"date";s:19:"2005-07-14 22:30:41";s:13:"timezone_type";i:3;s:8:"timezone";s:13:"Europe/London";}'));

// New style.
var_dump(unserialize('O:8:"DateTime":3:{s:4:"date";s:26:"2005-07-14 22:30:41.123456";s:13:"timezone_type";i:3;s:8:"timezone";s:13:"Europe/London";}'));
?>
--EXPECTF--
object(DateTime)#%d (3) {
  ["date"]=>
  string(26) "2005-07-14 22:30:41.000000"
  ["timezone_type"]=>
  int(3)
  ["timezone"]=>
  string(13) "Europe/London"
}
object(DateTime)#%d (3) {
  ["date"]=>
  string(26) "2005-07-14 22:30:41.123456"
  ["timezone_type"]=>
  int(3)
  ["timezone"]=>
  string(13) "Europe/London"
}
