--TEST--
Test DateInterval::__serialize and DateInterval::__unserialize
--FILE--
<?php
date_default_timezone_set("Europe/London");

$d = DateInterval::createFromDateString('next weekday 15:30');
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
$d = new DateInterval('P2Y4DT6H8M');
$d->__unserialize(
	[
		'from_string' => true,
		'date_string' => 'next weekday 15:30',
	]
);
var_dump($d);
?>
--EXPECTF--
Original object:
object(DateInterval)#1 (%d) {
  ["from_string"]=>
  bool(true)
  ["date_string"]=>
  string(18) "next weekday 15:30"
}


Serialised object:
string(92) "O:12:"DateInterval":2:{s:11:"from_string";b:1;s:11:"date_string";s:18:"next weekday 15:30";}"


Unserialised object:
object(DateInterval)#2 (2) {
  ["from_string"]=>
  bool(true)
  ["date_string"]=>
  string(18) "next weekday 15:30"
}


Calling __serialize manually:
array(2) {
  ["from_string"]=>
  bool(true)
  ["date_string"]=>
  string(18) "next weekday 15:30"
}


Calling __unserialize manually:
object(DateInterval)#3 (2) {
  ["from_string"]=>
  bool(true)
  ["date_string"]=>
  string(18) "next weekday 15:30"
}
