--TEST--
Test DateTimeZone::__serialize and DateTime::__unserialize
--FILE--
<?php
$tz = new DateTimeZone("CEST");
var_dump($tz->__serialize());

$tz = new DateTimeZone("UTC");
$tz->__unserialize(
	[
		'timezone_type' => 3,
		'timezone' => 'Europe/London',
	]
);
var_dump($tz);

$tz->__unserialize(
	[
		'timezone_type' => 2,
		'timezone' => 'CEST',
	]
);
var_dump($tz);

$tz->__unserialize(
	[
		'timezone_type' => 1,
		'timezone' => '+0130',
	]
);
var_dump($tz);

?>
--EXPECTF--
array(2) {
  ["timezone_type"]=>
  int(2)
  ["timezone"]=>
  string(4) "CEST"
}
object(DateTimeZone)#%d (%d) {
  ["timezone_type"]=>
  int(3)
  ["timezone"]=>
  string(13) "Europe/London"
}
object(DateTimeZone)#%d (%d) {
  ["timezone_type"]=>
  int(2)
  ["timezone"]=>
  string(4) "CEST"
}
object(DateTimeZone)#%d (%d) {
  ["timezone_type"]=>
  int(1)
  ["timezone"]=>
  string(6) "+01:30"
}
