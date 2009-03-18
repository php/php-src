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
object(DateTime)#%d (3) {
  [u"date"]=>
  unicode(19) "2005-07-14 22:30:41"
  [u"timezone_type"]=>
  int(3)
  [u"timezone"]=>
  unicode(13) "Europe/London"
}
unicode(118) "O:8:"DateTime":3:{U:4:"date";U:19:"2005-07-14 22:30:41";U:13:"timezone_type";i:3;U:8:"timezone";U:13:"Europe/London";}"
object(DateTime)#%d (3) {
  [u"date"]=>
  unicode(19) "2005-07-14 22:30:41"
  [u"timezone_type"]=>
  int(3)
  [u"timezone"]=>
  unicode(13) "Europe/London"
}
unicode(23) "July 14, 2005, 10:30 pm"
===DONE===