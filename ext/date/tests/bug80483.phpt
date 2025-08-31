--TEST--
Bug #80483 (DateTime Object with 5-digit year can't unserialized)
--INI--
date.timezone=UTC
--FILE--
<?php

$the_date = new DateTime();
$the_date->SetTime(0, 0, 0);
$the_date->SetDate(20201, 01, 01);

var_dump($the_date);

$serialized = serialize($the_date);
var_dump($serialized);
var_dump(unserialize($serialized));
?>
--EXPECTF--
object(DateTime)#%d (%d) {
  ["date"]=>
  string(28) "+20201-01-01 00:00:00.000000"
  ["timezone_type"]=>
  int(3)
  ["timezone"]=>
  string(3) "UTC"
}
string(116) "O:8:"DateTime":3:{s:4:"date";s:28:"+20201-01-01 00:00:00.000000";s:13:"timezone_type";i:3;s:8:"timezone";s:3:"UTC";}"
object(DateTime)#%d (%d) {
  ["date"]=>
  string(28) "+20201-01-01 00:00:00.000000"
  ["timezone_type"]=>
  int(3)
  ["timezone"]=>
  string(3) "UTC"
}
