--TEST--
Bug #54340 (DateTime::add() method bug)
--INI--
date.timezone=UTC
--FILE--
<?php
$interval = new DateInterval('P1D');

$dt = new DateTime('first day of January 2011');
var_dump($dt);

$dt->add($interval);
var_dump($dt);

$dt = new DateTime('first day of January 2011');

$dt->sub($interval);
var_dump($dt);
--EXPECT--
object(DateTime)#2 (3) {
  ["date"]=>
  string(19) "2011-01-01 00:00:00"
  ["timezone_type"]=>
  int(3)
  ["timezone"]=>
  string(3) "UTC"
}
object(DateTime)#2 (3) {
  ["date"]=>
  string(19) "2011-01-02 00:00:00"
  ["timezone_type"]=>
  int(3)
  ["timezone"]=>
  string(3) "UTC"
}
object(DateTime)#3 (3) {
  ["date"]=>
  string(19) "2010-12-31 00:00:00"
  ["timezone_type"]=>
  int(3)
  ["timezone"]=>
  string(3) "UTC"
}
