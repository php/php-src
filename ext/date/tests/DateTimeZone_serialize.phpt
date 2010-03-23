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
object(DateTimeZone)#%d (0) {
}
string(24) "O:12:"DateTimeZone":0:{}"
object(DateTimeZone)#%d (0) {
}

Warning: DateTimeZone::getName(): The DateTimeZone object has not been correctly initialized by its constructor in %s on line %d
bool(false)
===DONE===