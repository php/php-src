--TEST--
Test serialization of DateTime objects
--FILE--
<?php
//Set the default time zone
date_default_timezone_set("Europe/London");

$date1 = new DateTime("2005-07-14 22:30:41");
var_dump($date1);
$serialized = serialize($date1);
var_dump($serialized);

$date2 = unserialize($serialized);
var_dump($date2);
// Try to use unserialzied object
var_dump( $date2->format( "F j, Y, g:i a") );

?>
--EXPECTF--
object(DateTime)#%d (3) {
  ["date"]=>
  string(26) "2005-07-14 22:30:41.000000"
  ["timezone_type"]=>
  int(3)
  ["timezone"]=>
  string(13) "Europe/London"
}
string(125) "O:8:"DateTime":3:{s:4:"date";s:26:"2005-07-14 22:30:41.000000";s:13:"timezone_type";i:3;s:8:"timezone";s:13:"Europe/London";}"
object(DateTime)#%d (3) {
  ["date"]=>
  string(26) "2005-07-14 22:30:41.000000"
  ["timezone_type"]=>
  int(3)
  ["timezone"]=>
  string(13) "Europe/London"
}
string(23) "July 14, 2005, 10:30 pm"
