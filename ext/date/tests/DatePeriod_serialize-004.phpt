--TEST--
Test DatePeriod::__serialize and DatePeriod::__unserialize (start/recurrences)
--FILE--
<?php
date_default_timezone_set("Europe/London");

$s = new DateTimeImmutable("1978-12-22 09:15:00 Europe/Amsterdam");
$i = DateInterval::createFromDateString('first monday of next month');
$p = new DatePeriod($s, $i, 7, DatePeriod::EXCLUDE_START_DATE);
echo "Original object:\n";
var_dump($p);

echo "\n\nIterate of object:\n";
foreach ( $p as $d )
{
	echo $d->format(DateTime::ISO8601), "\n";
}

echo "\n\nSerialised object:\n";
$s = serialize($p);
var_dump($s);

echo "\n\nUnserialised object:\n";
$e = unserialize($s);
var_dump($e);

echo "\n\nCalling __serialize manually:\n";
var_dump($p->__serialize());

echo "\n\nIterate of unserialised object:\n";
foreach ( $p as $d )
{
	echo $d->format(DateTime::ISO8601), "\n";
}
?>
--EXPECTF--
Original object:
object(DatePeriod)#%d (%d) {
  ["start"]=>
  object(DateTimeImmutable)#%d (%d) {
    ["date"]=>
    string(26) "1978-12-22 09:15:00.000000"
    ["timezone_type"]=>
    int(3)
    ["timezone"]=>
    string(16) "Europe/Amsterdam"
  }
  ["current"]=>
  NULL
  ["end"]=>
  NULL
  ["interval"]=>
  object(DateTimeInterval)#%d (%d) {
    ["from_string"]=>
    bool(false)
    ["y"]=>
    int(0)
    ["m"]=>
    int(1)
    ["d"]=>
    int(0)
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
  }
  ["recurrences"]=>
  int(7)
  ["include_start_date"]=>
  bool(false)
  ["include_end_date"]=>
  bool(false)
}


Iterate of object:
1979-01-01T09:15:00+0100
1979-02-05T09:15:00+0100
1979-03-05T09:15:00+0100
1979-04-02T09:15:00+0200
1979-05-07T09:15:00+0200
1979-06-04T09:15:00+0200
1979-07-02T09:15:00+0200


Serialised object:
string(%d) "O:10:"DatePeriod":7:{s:5:"start";O:17:"DateTimeImmutable":3:{s:4:"date";s:26:"1978-12-22 09:15:00.000000";s:13:"timezone_type";i:3;s:8:"timezone";s:16:"Europe/Amsterdam";}s:7:"current";O:17:"DateTimeImmutable":3:{s:4:"date";s:26:"1979-08-06 09:15:00.000000";s:13:"timezone_type";i:3;s:8:"timezone";s:16:"Europe/Amsterdam";}s:3:"end";N;s:8:"interval";O:16:"DateTimeInterval":10:{s:11:"from_string";b:0;s:1:"y";i:0;s:1:"m";i:1;s:1:"d";i:0;s:1:"h";i:0;s:1:"i";i:0;s:1:"s";i:0;s:1:"f";d:0;s:6:"invert";i:0;s:4:"days";b:0;}s:11:"recurrences";i:7;s:18:"include_start_date";b:0;s:16:"include_end_date";b:0;}"


Unserialised object:
object(DatePeriod)#%d (%d) {
  ["start"]=>
  object(DateTimeImmutable)#%d (%d) {
    ["date"]=>
    string(26) "1978-12-22 09:15:00.000000"
    ["timezone_type"]=>
    int(3)
    ["timezone"]=>
    string(16) "Europe/Amsterdam"
  }
  ["current"]=>
  object(DateTimeImmutable)#%d (%d) {
    ["date"]=>
    string(26) "1979-08-06 09:15:00.000000"
    ["timezone_type"]=>
    int(3)
    ["timezone"]=>
    string(16) "Europe/Amsterdam"
  }
  ["end"]=>
  NULL
  ["interval"]=>
  object(DateTimeInterval)#%d (%d) {
    ["from_string"]=>
    bool(false)
    ["y"]=>
    int(0)
    ["m"]=>
    int(1)
    ["d"]=>
    int(0)
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
  }
  ["recurrences"]=>
  int(7)
  ["include_start_date"]=>
  bool(false)
  ["include_end_date"]=>
  bool(false)
}


Calling __serialize manually:
array(%d) {
  ["start"]=>
  object(DateTimeImmutable)#%d (%d) {
    ["date"]=>
    string(26) "1978-12-22 09:15:00.000000"
    ["timezone_type"]=>
    int(3)
    ["timezone"]=>
    string(16) "Europe/Amsterdam"
  }
  ["current"]=>
  object(DateTimeImmutable)#%d (%d) {
    ["date"]=>
    string(26) "1979-08-06 09:15:00.000000"
    ["timezone_type"]=>
    int(3)
    ["timezone"]=>
    string(16) "Europe/Amsterdam"
  }
  ["end"]=>
  NULL
  ["interval"]=>
  object(DateTimeInterval)#%d (%d) {
    ["from_string"]=>
    bool(false)
    ["y"]=>
    int(0)
    ["m"]=>
    int(1)
    ["d"]=>
    int(0)
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
  }
  ["recurrences"]=>
  int(7)
  ["include_start_date"]=>
  bool(false)
  ["include_end_date"]=>
  bool(false)
}


Iterate of unserialised object:
1979-01-01T09:15:00+0100
1979-02-05T09:15:00+0100
1979-03-05T09:15:00+0100
1979-04-02T09:15:00+0200
1979-05-07T09:15:00+0200
1979-06-04T09:15:00+0200
1979-07-02T09:15:00+0200
