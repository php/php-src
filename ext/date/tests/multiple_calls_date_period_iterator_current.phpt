--TEST--
Multiple calls to DatePeriod iterator current() leak objects
--FILE--
<?php
$start = new DateTime('2018-12-31 00:00:00');
$end   = new DateTime('2019-12-31 00:00:00');

$interval = new DateInterval('P1M');
$period = new DatePeriod($start, $interval, 1);

$iter = $period->getIterator();
var_dump($iter->current());
var_dump($iter->current());
$iter->current()->setTimestamp(0);
var_dump($iter->current());

?>
--EXPECTF--
object(DateTime)#%d (3) {
  ["date"]=>
  string(26) "2018-12-31 00:00:00.000000"
  ["timezone_type"]=>
  int(3)
  ["timezone"]=>
  string(3) "UTC"
}
object(DateTime)#%d (3) {
  ["date"]=>
  string(26) "2018-12-31 00:00:00.000000"
  ["timezone_type"]=>
  int(3)
  ["timezone"]=>
  string(3) "UTC"
}
object(DateTime)#%d (3) {
  ["date"]=>
  string(26) "2018-12-31 00:00:00.000000"
  ["timezone_type"]=>
  int(3)
  ["timezone"]=>
  string(3) "UTC"
}
