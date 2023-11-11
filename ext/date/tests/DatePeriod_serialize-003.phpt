--TEST--
Test DatePeriod::__serialize and DatePeriod::__unserialize (start/end)
--FILE--
<?php
date_default_timezone_set("Europe/London");

$s = new DateTimeImmutable("1978-12-22 09:15:00 Europe/Amsterdam");
$e = new DateTimeImmutable("2022-04-29 15:51:56 Europe/London");
$i = new DateInterval('P2Y6M');
$d = new DatePeriod($s, $i, $e, DatePeriod::EXCLUDE_START_DATE);
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

echo "\n\nIterate of unserialised object:\n";
foreach ( $e as $d )
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
  object(DateTimeImmutable)#%d (%d) {
    ["date"]=>
    string(26) "2022-04-29 15:51:56.000000"
    ["timezone_type"]=>
    int(3)
    ["timezone"]=>
    string(13) "Europe/London"
  }
  ["interval"]=>
  object(DateInterval)#%d (%d) {
    ["y"]=>
    int(2)
    ["m"]=>
    int(6)
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
    ["from_string"]=>
    bool(false)
  }
  ["recurrences"]=>
  int(0)
  ["include_start_date"]=>
  bool(false)
  ["include_end_date"]=>
  bool(false)
}


Serialised object:
string(%d) "O:10:"DatePeriod":7:{s:5:"start";O:17:"DateTimeImmutable":3:{s:4:"date";s:26:"1978-12-22 09:15:00.000000";s:13:"timezone_type";i:3;s:8:"timezone";s:16:"Europe/Amsterdam";}s:7:"current";N;s:3:"end";O:17:"DateTimeImmutable":3:{s:4:"date";s:26:"2022-04-29 15:51:56.000000";s:13:"timezone_type";i:3;s:8:"timezone";s:13:"Europe/London";}s:8:"interval";O:12:"DateInterval":10:{s:1:"y";i:2;s:1:"m";i:6;s:1:"d";i:0;s:1:"h";i:0;s:1:"i";i:0;s:1:"s";i:0;s:1:"f";d:0;s:6:"invert";i:0;s:4:"days";b:0;s:11:"from_string";b:0;}s:11:"recurrences";i:0;s:18:"include_start_date";b:0;s:16:"include_end_date";b:0;}"


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
  NULL
  ["end"]=>
  object(DateTimeImmutable)#%d (%d) {
    ["date"]=>
    string(26) "2022-04-29 15:51:56.000000"
    ["timezone_type"]=>
    int(3)
    ["timezone"]=>
    string(13) "Europe/London"
  }
  ["interval"]=>
  object(DateInterval)#%d (%d) {
    ["y"]=>
    int(2)
    ["m"]=>
    int(6)
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
    ["from_string"]=>
    bool(false)
  }
  ["recurrences"]=>
  int(0)
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
  NULL
  ["end"]=>
  object(DateTimeImmutable)#%d (%d) {
    ["date"]=>
    string(26) "2022-04-29 15:51:56.000000"
    ["timezone_type"]=>
    int(3)
    ["timezone"]=>
    string(13) "Europe/London"
  }
  ["interval"]=>
  object(DateInterval)#%d (%d) {
    ["y"]=>
    int(2)
    ["m"]=>
    int(6)
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
    ["from_string"]=>
    bool(false)
  }
  ["recurrences"]=>
  int(0)
  ["include_start_date"]=>
  bool(false)
  ["include_end_date"]=>
  bool(false)
}


Iterate of unserialised object:
1981-06-22T09:15:00+0200
1983-12-22T09:15:00+0100
1986-06-22T09:15:00+0200
1988-12-22T09:15:00+0100
1991-06-22T09:15:00+0200
1993-12-22T09:15:00+0100
1996-06-22T09:15:00+0200
1998-12-22T09:15:00+0100
2001-06-22T09:15:00+0200
2003-12-22T09:15:00+0100
2006-06-22T09:15:00+0200
2008-12-22T09:15:00+0100
2011-06-22T09:15:00+0200
2013-12-22T09:15:00+0100
2016-06-22T09:15:00+0200
2018-12-22T09:15:00+0100
2021-06-22T09:15:00+0200
