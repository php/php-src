--TEST--
Test DatePeriod::__serialize and DatePeriod::__unserialize (ISO String)
--FILE--
<?php
date_default_timezone_set("Europe/London");

$d = DatePeriod::createFromISO8601String('R4/2012-07-01T00:00:00Z/P7D');
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
object(DatePeriod)#%d (%d) {
  ["start"]=>
  object(DateTimeImmutable)#%d (%d) {
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
  object(DateInterval)#%d (%d) {
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
    ["date_string"]=>
    NULL
  }
  ["recurrences"]=>
  int(5)
  ["include_start_date"]=>
  bool(true)
  ["include_end_date"]=>
  bool(false)
}


Serialised object:
string(470) "O:10:"DatePeriod":7:{s:5:"start";O:17:"DateTimeImmutable":3:{s:4:"date";s:26:"2012-07-01 00:00:00.000000";s:13:"timezone_type";i:1;s:8:"timezone";s:6:"+00:00";}s:7:"current";N;s:3:"end";N;s:8:"interval";O:12:"DateInterval":11:{s:1:"y";i:0;s:1:"m";i:0;s:1:"d";i:7;s:1:"h";i:0;s:1:"i";i:0;s:1:"s";i:0;s:1:"f";d:0;s:6:"invert";i:0;s:4:"days";b:0;s:11:"from_string";b:0;s:11:"date_string";N;}s:11:"recurrences";i:5;s:18:"include_start_date";b:1;s:16:"include_end_date";b:0;}"


Unserialised object:
object(DatePeriod)#%d (%d) {
  ["start"]=>
  object(DateTimeImmutable)#%d (%d) {
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
  object(DateInterval)#%d (%d) {
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
    ["date_string"]=>
    NULL
  }
  ["recurrences"]=>
  int(5)
  ["include_start_date"]=>
  bool(true)
  ["include_end_date"]=>
  bool(false)
}


Calling __serialize manually:
array(%d) {
  ["start"]=>
  object(DateTimeImmutable)#%d (%d) {
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
  object(DateInterval)#%d (%d) {
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
    ["date_string"]=>
    NULL
  }
  ["recurrences"]=>
  int(5)
  ["include_start_date"]=>
  bool(true)
  ["include_end_date"]=>
  bool(false)
}
