--TEST--
Bug #48058 (Year formatter goes wrong with out-of-int range)
--INI--
date.timezone=UTC
--FILE--
<?php
date_default_timezone_set("Europe/London");
$tz = new DateTimeZone("Europe/London");
$tran = $tz->getTransitions();
var_dump( $tran[0] );

$base_time = '28 Feb 2008 12:00:00';
$dt = date_create( "$base_time +10000000000 years" );
echo date_format( $dt, DATE_ISO8601 );
?>
--EXPECTF--
array(5) {
  ["ts"]=>
  int(-%d)
  ["time"]=>
  string(%d) "%s"
  ["offset"]=>
  int(3600)
  ["isdst"]=>
  bool(true)
  ["abbr"]=>
  string(3) "BST"
}
10000002008-02-28T12:00:00+0000
