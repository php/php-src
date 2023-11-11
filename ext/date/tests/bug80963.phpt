--TEST--
Bug #80963: DateTimeZone::getTransitions() truncated
--INI--
date.timezone=UTC
--FILE--
<?php
$tzids = [ 'Europe/London', 'America/New_York', 'Europe/Berlin' ];

foreach ($tzids as $tzid)
{
	$tz = new DateTimeZone($tzid);
	$t  = $tz->getTransitions();
	var_dump(sizeof($t), end($t));
}
?>
--EXPECT--
int(243)
array(5) {
  ["ts"]=>
  int(2140045200)
  ["time"]=>
  string(25) "2037-10-25T01:00:00+00:00"
  ["offset"]=>
  int(0)
  ["isdst"]=>
  bool(false)
  ["abbr"]=>
  string(3) "GMT"
}
int(237)
array(5) {
  ["ts"]=>
  int(2140668000)
  ["time"]=>
  string(25) "2037-11-01T06:00:00+00:00"
  ["offset"]=>
  int(-18000)
  ["isdst"]=>
  bool(false)
  ["abbr"]=>
  string(3) "EST"
}
int(144)
array(5) {
  ["ts"]=>
  int(2140045200)
  ["time"]=>
  string(25) "2037-10-25T01:00:00+00:00"
  ["offset"]=>
  int(3600)
  ["isdst"]=>
  bool(false)
  ["abbr"]=>
  string(3) "CET"
}
