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
?>
--EXPECTF--
Original object:
object(DateInterval)#3 (10) {
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
  ["have_special_relative"]=>
  int(0)
}


Serialised object:
string(340) "O:12:"DateInterval":16:{s:1:"y";i:43;s:1:"m";i:3;s:1:"d";i:24;s:1:"h";i:1;s:1:"i";i:12;s:1:"s";i:27;s:1:"f";d:0;s:7:"weekday";i:0;s:16:"weekday_behavior";i:0;s:17:"first_last_day_of";i:0;s:6:"invert";i:0;s:4:"days";i:15820;s:12:"special_type";i:0;s:14:"special_amount";i:0;s:21:"have_weekday_relative";i:0;s:21:"have_special_relative";i:0;}"


Unserialised object:
object(DateInterval)#4 (10) {
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
  ["have_special_relative"]=>
  int(0)
}


Calling __serialize manually:
array(16) {
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
  ["weekday"]=>
  int(0)
  ["weekday_behavior"]=>
  int(0)
  ["first_last_day_of"]=>
  int(0)
  ["invert"]=>
  int(0)
  ["days"]=>
  int(15820)
  ["special_type"]=>
  int(0)
  ["special_amount"]=>
  int(0)
  ["have_weekday_relative"]=>
  int(0)
  ["have_special_relative"]=>
  int(0)
}


Calling __unserialize manually:
object(DateInterval)#5 (10) {
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
  ["have_special_relative"]=>
  int(0)
}
