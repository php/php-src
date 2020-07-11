--TEST--
Test serialization of DateTimeZone objects
--FILE--
<?php
//Set the default time zone
date_default_timezone_set("Europe/London");

$tz1 = new DateTimeZone("EST");
var_dump( $tz1 );
$serialized = serialize($tz1);
var_dump($serialized);

$tz2 = unserialize($serialized);
var_dump($tz2);
// Try to use unserialzied object
var_dump( $tz2->getName() );

?>
--EXPECTF--
object(DateTimeZone)#%d (2) {
  ["timezone_type"]=>
  int(2)
  ["timezone"]=>
  string(3) "EST"
}
string(74) "O:12:"DateTimeZone":2:{s:13:"timezone_type";i:2;s:8:"timezone";s:3:"EST";}"
object(DateTimeZone)#%d (2) {
  ["timezone_type"]=>
  int(2)
  ["timezone"]=>
  string(3) "EST"
}
string(3) "EST"
