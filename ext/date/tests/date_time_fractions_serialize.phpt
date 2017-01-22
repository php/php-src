--TEST--
Fractions with DateTime objects (Serialization)
--INI--
date.timezone=UTC
--FILE--
<?php
/* Normal creation */
$dt = date_create( "2016-10-03 12:47:18.819313" );

$s = serialize( $dt );
echo $s, "\n";

$u = unserialize( $s );
var_dump( $u );
?>
--EXPECTF--
O:8:"DateTime":3:{s:4:"date";s:26:"2016-10-03 12:47:18.819313";s:13:"timezone_type";i:3;s:8:"timezone";s:3:"UTC";}
object(DateTime)#2 (%d) {
  ["date"]=>
  string(26) "2016-10-03 12:47:18.819313"
  ["timezone_type"]=>
  int(3)
  ["timezone"]=>
  string(3) "UTC"
}
