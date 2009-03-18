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
===DONE=== 
--EXPECTF--
object(DateTime)#%d (0) {
}
string(19) "O:8:"DateTime":0:{}"
object(DateTime)#%d (0) {
}

Warning: DateTime::format(): The DateTime object has not been correctly initialized by its constructor in %s on line %d
bool(false)
===DONE===