--TEST--
Bug #66545 (DateTime)
--INI--
date.timezone=Europe/Paris
--FILE--
<?php

$debut = mktime(0, 0, 0, 10, 27, 2013);
$fin = mktime(23, 59, 59, 11, 10, 2013);

$d1 = new DateTime('now',new DateTimeZone('Europe/Paris'));
$d2 = new DateTime('now',new DateTimeZone('Europe/Paris'));
$d1->setTimestamp($debut);
$d2->setTimestamp($fin);
$diff = $d1->diff($d2);
var_dump($diff);
?>
--EXPECT--
object(DateTimeInterval)#3 (10) {
  ["from_string"]=>
  bool(false)
  ["y"]=>
  int(0)
  ["m"]=>
  int(0)
  ["d"]=>
  int(14)
  ["h"]=>
  int(23)
  ["i"]=>
  int(59)
  ["s"]=>
  int(59)
  ["f"]=>
  float(0)
  ["invert"]=>
  int(0)
  ["days"]=>
  int(14)
}
