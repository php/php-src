--TEST--
Test __set_state magic method for recreating a DateTimeImmutable object
--CREDITS--
Mark Baker mark@lange.demon.co.uk at the PHPNW2017 Conference for PHP Testfest 2017
--FILE--
<?php

$datettimeObject = new DateTimeImmutable('2017-10-06 23:30:00', new DateTimezone('UTC'));

$datetimeState = var_export($datettimeObject, true);

eval("\$datetimeObjectNew = {$datetimeState};");

var_dump($datetimeObjectNew);

?>
--EXPECTF--
object(DateTimeImmutable)#%d (3) {
  ["date"]=>
  string(26) "2017-10-06 23:30:00.000000"
  ["timezone_type"]=>
  int(3)
  ["timezone"]=>
  string(3) "UTC"
}
