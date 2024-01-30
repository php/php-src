--TEST--
Test for date_diff with timezone abbreviations.
--INI--
date.timezone=Europe/London
--FILE--
<?php
$start = new DateTime('2010-10-04 02:18:48 EDT');
$end   = new DateTime('2010-11-06 18:38:28 EDT');
$int = $start->diff($end);
var_dump($start);
var_dump($end);
var_dump($int);
?>
--EXPECTF--
object(DateTime)#1 (3) {
  ["date"]=>
  string(26) "2010-10-04 02:18:48.000000"
  ["timezone_type"]=>
  int(2)
  ["timezone"]=>
  string(3) "EDT"
}
object(DateTime)#2 (3) {
  ["date"]=>
  string(26) "2010-11-06 18:38:28.000000"
  ["timezone_type"]=>
  int(2)
  ["timezone"]=>
  string(3) "EDT"
}
object(DateTimeInterval)#%d (%d) {
  ["from_string"]=>
  bool(false)
  ["y"]=>
  int(0)
  ["m"]=>
  int(1)
  ["d"]=>
  int(2)
  ["h"]=>
  int(%d)
  ["i"]=>
  int(19)
  ["s"]=>
  int(40)
  ["f"]=>
  float(0)
  ["invert"]=>
  int(0)
  ["days"]=>
  int(33)
}
