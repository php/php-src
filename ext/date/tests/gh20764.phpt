--TEST--
GH-20764 (Timezone offset with seconds loses precision)
--FILE--
<?php

$tz = new DateTimeZone('+03:00:30');
$dt = new DateTimeImmutable('2025-04-01', $tz);
var_dump($dt->format('e'));
var_dump($dt);
var_dump(unserialize(serialize($dt))->getTimezone());

?>
--EXPECT--
string(9) "+03:00:30"
object(DateTimeImmutable)#2 (3) {
  ["date"]=>
  string(26) "2025-04-01 00:00:00.000000"
  ["timezone_type"]=>
  int(1)
  ["timezone"]=>
  string(9) "+03:00:30"
}
object(DateTimeZone)#4 (2) {
  ["timezone_type"]=>
  int(1)
  ["timezone"]=>
  string(9) "+03:00:30"
}
