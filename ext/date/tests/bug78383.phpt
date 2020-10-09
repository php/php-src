--TEST--
Bug #78383: Casting a DateTime to array no longer returns its properties
--FILE--
<?php

var_dump((array) new DateTime('2000-01-01 UTC'));
var_dump((array) new DateTimeZone('Europe/Berlin'));

?>
--EXPECT--
array(3) {
  ["date"]=>
  string(26) "2000-01-01 00:00:00.000000"
  ["timezone_type"]=>
  int(3)
  ["timezone"]=>
  string(3) "UTC"
}
array(2) {
  ["timezone_type"]=>
  int(3)
  ["timezone"]=>
  string(13) "Europe/Berlin"
}
