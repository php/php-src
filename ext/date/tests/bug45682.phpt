--TEST--
Bug #45682 (Unable to var_dump(DateInterval))
--INI--
date.timezone=UTC
--FILE--
<?php

$date = new DateTime("28-July-2008");
$other = new DateTime("31-July-2008");

$diff = date_diff($date, $other);

var_dump($diff);
--EXPECTF--
object(DateInterval)#%d (16) {
  ["y"]=>
  int(0)
  ["m"]=>
  int(0)
  ["d"]=>
  int(3)
  ["h"]=>
  int(0)
  ["i"]=>
  int(0)
  ["s"]=>
  int(0)
  ["f"]=>
  float(0)
  ["weekday"]=>
  int(0)
  ["weekday_behavior"]=>
  int(0)
  ["first_last_day_of"]=>
  int(0)
  ["invert"]=>
  int(0)
  ["days"]=>
  int(3)
  ["special_type"]=>
  int(0)
  ["special_amount"]=>
  int(0)
  ["have_weekday_relative"]=>
  int(0)
  ["have_special_relative"]=>
  int(0)
}
