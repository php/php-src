--TEST--
Test serialization of DateTimeZone objects
--FILE--
<?php
//Set the default time zone 
date_default_timezone_set("Europe/London");

$tz1 = new DateTimeZone("America/New_York");
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
  int(3)
  ["timezone"]=>
  string(16) "America/New_York"
}
string(88) "O:12:"DateTimeZone":2:{s:13:"timezone_type";i:3;s:8:"timezone";s:16:"America/New_York";}"
object(DateTimeZone)#%d (2) {
  ["timezone_type"]=>
  int(3)
  ["timezone"]=>
  string(16) "America/New_York"
}
string(16) "America/New_York"
===DONE===
