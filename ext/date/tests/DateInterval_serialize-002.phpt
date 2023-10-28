--TEST--
Test DateInterval::__serialize and DateInterval::__unserialize
--FILE--
<?php
date_default_timezone_set("Europe/London");

$d1 = new DateTimeImmutable("1978-12-22 09:15 CET");
$d2 = new DateTimeImmutable("2022-04-15 10:27:27 BST");

$d = $d1->diff($d2);
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
		'y' => 43,
		'm' =>  3,
		'd' => 24,
		'h' =>  1,
		'i' => 12,
		's' => 27,
		'f' => 0.654321,
		'days' => 15820,
	]
);
var_dump($d);

echo "\n\nUsed serialised interval:\n";
$now = new DateTimeImmutable("2022-04-15 10:27:27 BST");
var_dump($now->add($e));
var_dump($now->sub($e));
?>
--EXPECTF--
Original object:
object(DateInterval)#3 (11) {
  ["y"]=>
  int(43)
  ["m"]=>
  int(3)
  ["d"]=>
  int(24)
  ["h"]=>
  int(1)
  ["i"]=>
  int(12)
  ["s"]=>
  int(27)
  ["f"]=>
  float(0)
  ["invert"]=>
  int(0)
  ["days"]=>
  int(15820)
  ["from_string"]=>
  bool(false)
  ["date_string"]=>
  NULL
}


Serialised object:
string(193) "O:12:"DateInterval":11:{s:1:"y";i:43;s:1:"m";i:3;s:1:"d";i:24;s:1:"h";i:1;s:1:"i";i:12;s:1:"s";i:27;s:1:"f";d:0;s:6:"invert";i:0;s:4:"days";i:15820;s:11:"from_string";b:0;s:11:"date_string";N;}"


Unserialised object:
object(DateInterval)#4 (11) {
  ["y"]=>
  int(43)
  ["m"]=>
  int(3)
  ["d"]=>
  int(24)
  ["h"]=>
  int(1)
  ["i"]=>
  int(12)
  ["s"]=>
  int(27)
  ["f"]=>
  float(0)
  ["invert"]=>
  int(0)
  ["days"]=>
  int(15820)
  ["from_string"]=>
  bool(false)
  ["date_string"]=>
  NULL
}


Calling __serialize manually:
array(%d) {
  ["y"]=>
  int(43)
  ["m"]=>
  int(3)
  ["d"]=>
  int(24)
  ["h"]=>
  int(1)
  ["i"]=>
  int(12)
  ["s"]=>
  int(27)
  ["f"]=>
  float(0)
  ["invert"]=>
  int(0)
  ["days"]=>
  int(15820)
  ["from_string"]=>
  bool(false)
  ["date_string"]=>
  NULL
}


Calling __unserialize manually:
object(DateInterval)#5 (11) {
  ["y"]=>
  int(43)
  ["m"]=>
  int(3)
  ["d"]=>
  int(24)
  ["h"]=>
  int(1)
  ["i"]=>
  int(12)
  ["s"]=>
  int(27)
  ["f"]=>
  float(0.654321)
  ["invert"]=>
  int(0)
  ["days"]=>
  int(15820)
  ["from_string"]=>
  bool(false)
  ["date_string"]=>
  NULL
}


Used serialised interval:
object(DateTimeImmutable)#6 (3) {
  ["date"]=>
  string(26) "2065-08-08 11:39:54.000000"
  ["timezone_type"]=>
  int(2)
  ["timezone"]=>
  string(3) "BST"
}
object(DateTimeImmutable)#6 (3) {
  ["date"]=>
  string(26) "1978-12-22 09:15:00.000000"
  ["timezone_type"]=>
  int(2)
  ["timezone"]=>
  string(3) "BST"
}
