--TEST--
Bug GH-11368: Date modify returns invalid datetime
--INI--
date.timezone=UTC
--FILE--
<?php

$datetime = date_create('2023-06-04');

$datetime->setTime(1,1,1,1 /* If set to any other number, it works fine */);
var_dump($datetime);

$datetime->modify('-100 ms');
var_dump($datetime);

?>
--EXPECTF--
object(DateTime)#1 (3) {
  ["date"]=>
  string(26) "2023-06-04 01:01:01.000001"
  ["timezone_type"]=>
  int(3)
  ["timezone"]=>
  string(3) "UTC"
}
object(DateTime)#1 (3) {
  ["date"]=>
  string(26) "2023-06-04 01:01:00.900001"
  ["timezone_type"]=>
  int(3)
  ["timezone"]=>
  string(3) "UTC"
}
