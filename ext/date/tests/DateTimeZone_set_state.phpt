--TEST--
Test __set_state magic method for recreating a DateTimeZone object
--CREDITS--
Mark Baker mark@lange.demon.co.uk at the PHPNW2017 Conference for PHP Testfest 2017
--FILE--
<?php

$datetimezoneObject = new DateTimezone('UTC');

$datetimezoneState = var_export($datetimezoneObject, true);

eval("\$datetimezoneObjectNew = {$datetimezoneState};");

var_dump($datetimezoneObjectNew);

?>
--EXPECTF--
object(DateTimeZone)#%d (2) {
  ["timezone_type"]=>
  int(3)
  ["timezone"]=>
  string(3) "UTC"
}
