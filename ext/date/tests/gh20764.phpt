--TEST--
GH-20764 (Timezone offset with seconds loses precision)
--FILE--
<?php

$timezones = [
    '+03:00:30',
    '-03:00:30',
];

foreach ($timezones as $timezone) {
  echo "--- Testing timezone $timezone ---\n";
  $tz = new DateTimeZone($timezone);
  $dt = new DateTimeImmutable('2025-04-01', $tz);
  var_dump($dt->format('e'));
  var_dump($dt);
  var_dump(unserialize(serialize($dt))->getTimezone());
}

?>
--EXPECTF--
--- Testing timezone +03:00:30 ---
string(9) "+03:00:30"
object(DateTimeImmutable)#%d (3) {
  ["date"]=>
  string(26) "2025-04-01 00:00:00.000000"
  ["timezone_type"]=>
  int(1)
  ["timezone"]=>
  string(9) "+03:00:30"
}
object(DateTimeZone)#%d (2) {
  ["timezone_type"]=>
  int(1)
  ["timezone"]=>
  string(9) "+03:00:30"
}
--- Testing timezone -03:00:30 ---
string(9) "-03:00:30"
object(DateTimeImmutable)#%d (3) {
  ["date"]=>
  string(26) "2025-04-01 00:00:00.000000"
  ["timezone_type"]=>
  int(1)
  ["timezone"]=>
  string(9) "-03:00:30"
}
object(DateTimeZone)#%d (2) {
  ["timezone_type"]=>
  int(1)
  ["timezone"]=>
  string(9) "-03:00:30"
}
