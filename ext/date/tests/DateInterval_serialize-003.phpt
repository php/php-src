--TEST--
Test DateInterval::__serialize and DateInterval::__unserialize
--FILE--
<?php
date_default_timezone_set("Europe/London");

$d = DateInterval::createFromDateString('next weekday');
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
		'date_string' => 'next weekday',
	]
);
var_dump($d);

echo "\n\nUsed serialised interval:\n";
$now = new DateTimeImmutable("2022-04-22 16:25:11 BST");
var_dump($now->add($e));
try {
	var_dump($now->sub($e));
} catch (DateInvalidOperationException $e) {
	echo $e::class, ': ', $e->getMessage(), "\n";
}
?>
--EXPECTF--
Original object:
object(DateTimeStringInterval)#1 (%d) {
  ["from_string"]=>
  bool(true)
  ["date_string"]=>
  string(%d) "next weekday"
}


Serialised object:
string(96) "O:22:"DateTimeStringInterval":2:{s:11:"from_string";b:1;s:11:"date_string";s:12:"next weekday";}"


Unserialised object:
object(DateTimeStringInterval)#2 (%d) {
  ["from_string"]=>
  bool(true)
  ["date_string"]=>
  string(%d) "next weekday"
}


Calling __serialize manually:
array(%d) {
  ["from_string"]=>
  bool(true)
  ["date_string"]=>
  string(%d) "next weekday"
}


Calling __unserialize manually:
object(DateInterval)#3 (%d) {
  ["from_string"]=>
  bool(true)
  ["date_string"]=>
  string(%d) "next weekday"
}


Used serialised interval:
object(DateTimeImmutable)#4 (%d) {
  ["date"]=>
  string(26) "2022-04-25 16:25:11.000000"
  ["timezone_type"]=>
  int(2)
  ["timezone"]=>
  string(3) "BST"
}
DateInvalidOperationException: DateTimeImmutable::sub(): Only non-special relative time specifications are supported for subtraction
