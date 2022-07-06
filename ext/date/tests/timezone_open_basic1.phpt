--TEST--
Test timezone_open() function : basic functionality
--FILE--
<?php
echo "*** Testing timezone_open() : basic functionality ***\n";

var_dump( timezone_open("GMT") );
var_dump( timezone_open("Europe/London") );
var_dump( timezone_open("America/Los_Angeles") );

?>
--EXPECTF--
*** Testing timezone_open() : basic functionality ***
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
