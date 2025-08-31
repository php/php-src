--TEST--
Bug #74671 (DST timezone abbreviation has incorrect offset)
--FILE--
<?php
$dt = new DateTime(
    '2017-05-16T10:11:32',
    new DateTimeZone('CEST')
);
var_dump($dt);
var_dump($dt->format('c'));
?>
--EXPECTF--
object(DateTime)#%d (%d) {
  ["date"]=>
  string(26) "2017-05-16 10:11:32.000000"
  ["timezone_type"]=>
  int(2)
  ["timezone"]=>
  string(4) "CEST"
}
string(25) "2017-05-16T10:11:32+02:00"
