--TEST--
Test date_create() function : basic functionality
--FILE--
<?php
//Set the default time zone
date_default_timezone_set("Europe/London");

echo "*** Testing date_create() : basic functionality ***\n";

var_dump( date_create() );

var_dump( date_create("GMT") );
var_dump( date_create("2005-07-14 22:30:41") );
var_dump( date_create("2005-07-14 22:30:41 GMT") );

?>
--EXPECTF--
*** Testing date_create() : basic functionality ***
object(DateTime)#%d (3) {
  ["date"]=>
  string(26) "%s"
  ["timezone_type"]=>
  int(3)
  ["timezone"]=>
  string(13) "Europe/London"
}
object(DateTime)#%d (3) {
  ["date"]=>
  string(26) "%s"
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
