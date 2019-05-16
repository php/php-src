--TEST--
Fractions with DateTime objects (create_from_format)
--INI--
date.timezone=UTC
--FILE--
<?php
$dt = date_create_from_format( "Y-m-d H:i:s.u", "2016-10-03 12:47:18.819313" );
var_dump( $dt );

$dt = date_create_from_format( "U.u", "1475500799.176312" );
var_dump( $dt );
?>
--EXPECTF--
object(DateTime)#%d (%d) {
  ["date"]=>
  string(26) "2016-10-03 12:47:18.819313"
  ["timezone_type"]=>
  int(3)
  ["timezone"]=>
  string(3) "UTC"
}
object(DateTime)#%d (%d) {
  ["date"]=>
  string(26) "2016-10-03 13:19:59.176312"
  ["timezone_type"]=>
  int(1)
  ["timezone"]=>
  string(6) "+00:00"
}
