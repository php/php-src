--TEST--
Test DatePeriod::__serialize and DatePeriod::__unserialize (ISO String)
--FILE--
<?php
date_default_timezone_set("Europe/London");

$d = new DatePeriod('R4/2012-07-01T00:00:00Z/P7D');
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

?>
--EXPECTF--
Original object:
object(DatePeriod)#1 (6) {
  ["start"]=>
  object(DateTime)#2 (3) {
    ["date"]=>
    string(26) "2012-07-01 00:00:00.000000"
    ["timezone_type"]=>
    int(1)
    ["timezone"]=>
    string(6) "+00:00"
  }
  ["current"]=>
  NULL
  ["end"]=>
  NULL
  ["interval"]=>
  object(DateInterval)#3 (10) {
    ["y"]=>
    int(0)
    ["m"]=>
    int(0)
    ["d"]=>
    int(7)
    ["h"]=>
    int(0)
    ["i"]=>
    int(0)
    ["s"]=>
    int(0)
    ["f"]=>
    float(0)
    ["invert"]=>
    int(0)
    ["days"]=>
    bool(false)
    ["from_string"]=>
    bool(false)
  }
  ["recurrences"]=>
  int(5)
  ["include_start_date"]=>
  bool(true)
}


Serialised object:
string(411) "O:10:"DatePeriod":6:{s:5:"start";O:8:"DateTime":3:{s:4:"date";s:26:"2012-07-01 00:00:00.000000";s:13:"timezone_type";i:1;s:8:"timezone";s:6:"+00:00";}s:7:"current";N;s:3:"end";N;s:8:"interval";O:12:"DateInterval":10:{s:1:"y";i:0;s:1:"m";i:0;s:1:"d";i:7;s:1:"h";i:0;s:1:"i";i:0;s:1:"s";i:0;s:1:"f";d:0;s:6:"invert";i:0;s:4:"days";b:0;s:11:"from_string";b:0;}s:11:"recurrences";i:5;s:18:"include_start_date";b:1;}"


Unserialised object:
object(DatePeriod)#5 (6) {
  ["start"]=>
  object(DateTime)#6 (3) {
    ["date"]=>
    string(26) "2012-07-01 00:00:00.000000"
    ["timezone_type"]=>
    int(1)
    ["timezone"]=>
    string(6) "+00:00"
  }
  ["current"]=>
  NULL
  ["end"]=>
  NULL
  ["interval"]=>
  object(DateInterval)#4 (10) {
    ["y"]=>
    int(0)
    ["m"]=>
    int(0)
    ["d"]=>
    int(7)
    ["h"]=>
    int(0)
    ["i"]=>
    int(0)
    ["s"]=>
    int(0)
    ["f"]=>
    float(0)
    ["invert"]=>
    int(0)
    ["days"]=>
    bool(false)
    ["from_string"]=>
    bool(false)
  }
  ["recurrences"]=>
  int(5)
  ["include_start_date"]=>
  bool(true)
}


Calling __serialize manually:
array(6) {
  ["start"]=>
  object(DateTime)#7 (3) {
    ["date"]=>
    string(26) "2012-07-01 00:00:00.000000"
    ["timezone_type"]=>
    int(1)
    ["timezone"]=>
    string(6) "+00:00"
  }
  ["current"]=>
  NULL
  ["end"]=>
  NULL
  ["interval"]=>
  object(DateInterval)#8 (10) {
    ["y"]=>
    int(0)
    ["m"]=>
    int(0)
    ["d"]=>
    int(7)
    ["h"]=>
    int(0)
    ["i"]=>
    int(0)
    ["s"]=>
    int(0)
    ["f"]=>
    float(0)
    ["invert"]=>
    int(0)
    ["days"]=>
    bool(false)
    ["from_string"]=>
    bool(false)
  }
  ["recurrences"]=>
  int(5)
  ["include_start_date"]=>
  bool(true)
}
