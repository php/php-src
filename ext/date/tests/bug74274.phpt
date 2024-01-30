--TEST--
Bug #74274 (Handling DST transitions correctly)
--FILE--
<?php
$tz = new DateTimeZone("Europe/Paris");
$startDate = new \DateTime('2018-10-28 00:00:00', $tz);
$endDateBuggy = new \DateTime('2018-10-29 23:00:00', $tz);

var_dump($startDate->diff($endDateBuggy));
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
  int(1)
  ["h"]=>
  int(23)
  ["i"]=>
  int(0)
  ["s"]=>
  int(0)
  ["f"]=>
  float(0)
  ["invert"]=>
  int(0)
  ["days"]=>
  int(1)
}