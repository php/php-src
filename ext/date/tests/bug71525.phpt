--TEST--
Bug #71525 (Calls to date_modify will mutate timelib_rel_time, causing date_date_set issues)
--INI--
date.timezone=UTC
--FILE--
<?php
$date = new DateTime('2011-12-25 00:00:00');
$date->modify('first day of next month');
$date->setDate('2012', '1', '29');
var_dump($date);

--EXPECTF--
object(DateTime)#%d (3) {
  ["date"]=>
  string(26) "2012-01-29 00:00:00.000000"
  ["timezone_type"]=>
  int(3)
  ["timezone"]=>
  string(3) "UTC"
}
