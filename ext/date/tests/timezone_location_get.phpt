--TEST--
timezone_location_get: Test that timezone_location_get returns a correct array of information
--CREDITS--
Havard Eide <nucleuz@gmail.com>
#PHPTestFest2009 Norway 2009-06-09 \o/
--INI--
date.timezone=UTC
--FILE--
<?php
$location = timezone_location_get(new DateTimeZone("Europe/Oslo"));
var_dump($location);
?>
--EXPECTF--
array(4) {
  ["country_code"]=>
  string(2) "NO"
  ["latitude"]=>
  float(59.9166%d)
  ["longitude"]=>
  float(10.75)
  ["comments"]=>
  string(0) ""
}
