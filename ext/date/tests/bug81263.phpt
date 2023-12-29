--TEST--
Bug #81263 (Wrong result from DateTimeImmutable::diff)
--FILE--
<?php

$dt1 = new DateTimeImmutable('2020-07-19 18:30:00', new DateTimeZone('Europe/Berlin'));
$dt2 = new DateTimeImmutable('2020-07-19 16:30:00', new DateTimeZone('UTC'));

var_dump($dt2->diff($dt1));
var_dump($dt1->diff($dt2));
?>
--EXPECTF--
object(DateTimeInterval)#%d (%d) {
  ["from_string"]=>
  bool(false)
  ["y"]=>
  int(0)
  ["m"]=>
  int(0)
  ["d"]=>
  int(0)
  ["h"]=>
  int(0)
  ["i"]=>
  int(0)
  ["s"]=>
  int(0)
  ["f"]=>
  float(0)
  ["invert"]=>
  int(0)
  ["days"]=>
  int(0)
}
object(DateTimeInterval)#%d (%d) {
  ["from_string"]=>
  bool(false)
  ["y"]=>
  int(0)
  ["m"]=>
  int(0)
  ["d"]=>
  int(0)
  ["h"]=>
  int(0)
  ["i"]=>
  int(0)
  ["s"]=>
  int(0)
  ["f"]=>
  float(0)
  ["invert"]=>
  int(0)
  ["days"]=>
  int(0)
}
