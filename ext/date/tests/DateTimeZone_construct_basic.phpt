--TEST--
Test new DateTimeZone() : basic functionality
--FILE--
<?php
/* Prototype  : DateTimeZone::__construct  ( string $timezone  )
 * Description: Returns new DateTimeZone object
 * Source code: ext/date/php_date.c
 * Alias to functions:
 */

//Set the default time zone
date_default_timezone_set("Europe/London");

echo "*** Testing new DateTimeZone() : basic functionality ***\n";

var_dump( new DateTimeZone("GMT") );
var_dump( new DateTimeZone("Europe/London") );
var_dump( new DateTimeZone("America/Los_Angeles") );

?>
===DONE===
--EXPECTF--
*** Testing new DateTimeZone() : basic functionality ***
object(DateTimeZone)#%d (2) {
  ["timezone_type"]=>
  int(2)
  ["timezone"]=>
  string(3) "GMT"
}
object(DateTimeZone)#%d (2) {
  ["timezone_type"]=>
  int(3)
  ["timezone"]=>
  string(13) "Europe/London"
}
object(DateTimeZone)#%d (2) {
  ["timezone_type"]=>
  int(3)
  ["timezone"]=>
  string(19) "America/Los_Angeles"
}
===DONE===
