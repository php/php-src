--TEST--
Test timezone_identifiers_list() function : basic functionality
--FILE--
<?php
echo "*** Testing timezone_identifiers_list() : basic functionality ***\n";

//Set the default time zone
date_default_timezone_set("GMT");

$zones = timezone_identifiers_list();
echo "Check return tpe is ARRAY\n";
var_dump(is_array($zones));

echo "Check array contains some common timezones\n";
var_dump(in_array("Europe/London", $zones));
var_dump(in_array("America/New_York", $zones));
var_dump(in_array("UTC", $zones));

?>
--EXPECT--
*** Testing timezone_identifiers_list() : basic functionality ***
Check return tpe is ARRAY
bool(true)
Check array contains some common timezones
bool(true)
bool(true)
bool(true)
	
