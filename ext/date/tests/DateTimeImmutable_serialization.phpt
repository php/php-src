--TEST--
Test DateTimeImmutable::__serialize and DateTimeImmutable::__unserialize
--FILE--
<?php
//Set the default time zone
date_default_timezone_set("Europe/London");

$d = new DateTimeImmutable("2022-04-14 11:27:42");
echo "Original object:\n";
var_dump($d);

echo "\n\nSerialised object:\n";
$s = serialize($d);
var_dump($s);

echo "\n\nUnserialised object:\n";
$e = unserialize($s);
var_dump($e);

echo "\n\nCalling __serialize manually:\n";
var_dump($d->__serialize());

echo "\n\nCalling __unserialize manually:\n";
$d = new DateTimeImmutable;
$d->__unserialize(
	[
		'date' => '2022-04-14 11:27:42.541106',
		'timezone_type' => 3,
		'timezone' => 'UTC',
	]
);
var_dump($d);

echo "\n\nCalling __unserialize a few more times, with abbreviations:\n";
$d->__unserialize(
	[
		'date' => '2022-04-14 11:27:42.541106',
		'timezone_type' => 2,
		'timezone' => 'CEST',
	]
);
var_dump($d);
$d->__unserialize(
	[
		'date' => '2022-04-14 11:27:42.541106',
		'timezone_type' => 1,
		'timezone' => '+0130',
	]
);
var_dump($d);

?>
--EXPECTF--
Original object:
object(DateTimeImmutable)#%d (%d) {
  ["date"]=>
  string(26) "2022-04-14 11:27:42.000000"
  ["timezone_type"]=>
  int(3)
  ["timezone"]=>
  string(13) "Europe/London"
}


Serialised object:
string(135) "O:17:"DateTimeImmutable":3:{s:4:"date";s:26:"2022-04-14 11:27:42.000000";s:13:"timezone_type";i:3;s:8:"timezone";s:13:"Europe/London";}"


Unserialised object:
object(DateTimeImmutable)#%d (%d) {
  ["date"]=>
  string(26) "2022-04-14 11:27:42.000000"
  ["timezone_type"]=>
  int(3)
  ["timezone"]=>
  string(13) "Europe/London"
}


Calling __serialize manually:
array(3) {
  ["date"]=>
  string(26) "2022-04-14 11:27:42.000000"
  ["timezone_type"]=>
  int(3)
  ["timezone"]=>
  string(13) "Europe/London"
}


Calling __unserialize manually:
object(DateTimeImmutable)#%d (%d) {
  ["date"]=>
  string(26) "2022-04-14 11:27:42.541106"
  ["timezone_type"]=>
  int(3)
  ["timezone"]=>
  string(3) "UTC"
}


Calling __unserialize a few more times, with abbreviations:
object(DateTimeImmutable)#%d (%d) {
  ["date"]=>
  string(26) "2022-04-14 11:27:42.541106"
  ["timezone_type"]=>
  int(2)
  ["timezone"]=>
  string(4) "CEST"
}
object(DateTimeImmutable)#%d (%d) {
  ["date"]=>
  string(26) "2022-04-14 11:27:42.541106"
  ["timezone_type"]=>
  int(1)
  ["timezone"]=>
  string(6) "+01:30"
}
