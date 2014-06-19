--TEST--
Test serialization of DateTimeZone objects
--FILE--
<?php
//Set the default time zone 
date_default_timezone_set("Europe/London");

$tz1 = date_create("2012-01-01 10:00 +1:00")->getTimezone();
var_dump( $tz1 );
$serialized = serialize($tz1);
var_dump($serialized); 

$tz2 = unserialize($serialized);
var_dump($tz2);
// Try to use unserialzied object 
var_dump( $tz2->getName() ); 

?>
===DONE=== 
--EXPECTF--
object(DateTimeZone)#%d (2) {
  ["timezone_type"]=>
  int(1)
  ["timezone"]=>
  string(6) "+01:00"
}
string(77) "O:12:"DateTimeZone":2:{s:13:"timezone_type";i:1;s:8:"timezone";s:6:"+01:00";}"
object(DateTimeZone)#%d (2) {
  ["timezone_type"]=>
  int(1)
  ["timezone"]=>
  string(6) "+01:00"
}
string(6) "+01:00"
===DONE===
