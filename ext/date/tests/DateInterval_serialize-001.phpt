--TEST--
Test DateInterval::__serialize and DateInterval::__unserialize
--FILE--
<?php
date_default_timezone_set("Europe/London");

$d = new DateInterval('P2Y4DT6H8M');
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

echo "\n\nUsed serialised interval:\n";
$now = new DateTimeImmutable("2022-04-22 16:25:11 BST");
var_dump($now->add($e));
var_dump($now->sub($e));
?>
--EXPECTF--
Original object:
object(DateInterval)#1 (%d) {
  ["from_string"]=>
  bool(false)
  ["y"]=>
  int(2)
  ["m"]=>
  int(0)
  ["d"]=>
  int(4)
  ["h"]=>
  int(6)
  ["i"]=>
  int(8)
  ["s"]=>
  int(0)
  ["f"]=>
  float(0)
  ["invert"]=>
  int(0)
  ["days"]=>
  bool(false)
}


Serialised object:
string(164) "O:12:"DateInterval":10:{s:11:"from_string";b:0;s:1:"y";i:2;s:1:"m";i:0;s:1:"d";i:4;s:1:"h";i:6;s:1:"i";i:8;s:1:"s";i:0;s:1:"f";d:0;s:6:"invert";i:0;s:4:"days";b:0;}"


Unserialised object:
object(DateInterval)#2 (%d) {
  ["from_string"]=>
  bool(false)
  ["y"]=>
  int(2)
  ["m"]=>
  int(0)
  ["d"]=>
  int(4)
  ["h"]=>
  int(6)
  ["i"]=>
  int(8)
  ["s"]=>
  int(0)
  ["f"]=>
  float(0)
  ["invert"]=>
  int(0)
  ["days"]=>
  bool(false)
}


Calling __serialize manually:
array(%d) {
  ["from_string"]=>
  bool(false)
  ["y"]=>
  int(2)
  ["m"]=>
  int(0)
  ["d"]=>
  int(4)
  ["h"]=>
  int(6)
  ["i"]=>
  int(8)
  ["s"]=>
  int(0)
  ["f"]=>
  float(0)
  ["invert"]=>
  int(0)
  ["days"]=>
  bool(false)
}


Used serialised interval:
object(DateTimeImmutable)#4 (%d) {
  ["date"]=>
  string(26) "2024-04-26 22:33:11.000000"
  ["timezone_type"]=>
  int(2)
  ["timezone"]=>
  string(3) "BST"
}
object(DateTimeImmutable)#4 (%d) {
  ["date"]=>
  string(26) "2020-04-18 10:17:11.000000"
  ["timezone_type"]=>
  int(2)
  ["timezone"]=>
  string(3) "BST"
}
