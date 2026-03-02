--TEST--
Test new DateTime() : basic functionality
--FILE--
<?php
//Set the default time zone
date_default_timezone_set("Europe/London");

echo "*** Testing new DateTime() : basic functionality ***\n";

var_dump( new DateTime('') );

var_dump( new DateTime("GMT") );
var_dump( new DateTime("2005-07-14 22:30:41") );
var_dump( new DateTime("2005-07-14 22:30:41 GMT") );

?>
--EXPECTF--
*** Testing new DateTime() : basic functionality ***
object(DateTime)#%d (3) {
  ["date"]=>
  string(%d) "%s"
  ["timezone_type"]=>
  int(3)
  ["timezone"]=>
  string(13) "Europe/London"
}
object(DateTime)#%d (3) {
  ["date"]=>
  string(%d) "%s"
  ["timezone_type"]=>
  int(2)
  ["timezone"]=>
  string(3) "GMT"
}
object(DateTime)#%d (3) {
  ["date"]=>
  string(26) "2005-07-14 22:30:41.000000"
  ["timezone_type"]=>
  int(3)
  ["timezone"]=>
  string(13) "Europe/London"
}
object(DateTime)#%d (3) {
  ["date"]=>
  string(26) "2005-07-14 22:30:41.000000"
  ["timezone_type"]=>
  int(2)
  ["timezone"]=>
  string(3) "GMT"
}
