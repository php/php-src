--TEST--
timezone_identifiers_list()
--FILE--
<?php
$a = timezone_identifiers_list();
$b = timezone_identifiers_list( DateTimezone::AMERICA );
$c = timezone_identifiers_list( DateTimezone::ALL_WITH_BC );
$d = timezone_identifiers_list( DateTimezone::EUROPE | DateTimezone::UTC );

echo in_array( "Europe/Oslo", $a ) ? "found" : "notfound", "\n";
echo in_array( "Europe/Oslo", $b ) ? "found" : "notfound", "\n";
echo in_array( "Europe/Oslo", $c ) ? "found" : "notfound", "\n";
echo in_array( "Europe/Oslo", $d ) ? "found" : "notfound", "\n\n";

echo in_array( "America/New_York", $a ) ? "found" : "notfound", "\n";
echo in_array( "America/New_York", $b ) ? "found" : "notfound", "\n";
echo in_array( "America/New_York", $c ) ? "found" : "notfound", "\n";
echo in_array( "America/New_York", $d ) ? "found" : "notfound", "\n\n";

echo in_array( "UTC", $a ) ? "found" : "notfound", "\n";
echo in_array( "UTC", $b ) ? "found" : "notfound", "\n";
echo in_array( "UTC", $c ) ? "found" : "notfound", "\n";
echo in_array( "UTC", $d ) ? "found" : "notfound", "\n\n";

echo in_array( "US/Eastern", $a ) ? "found" : "notfound", "\n";
echo in_array( "US/Eastern", $b ) ? "found" : "notfound", "\n";
echo in_array( "US/Eastern", $c ) ? "found" : "notfound", "\n";
echo in_array( "US/Eastern", $d ) ? "found" : "notfound", "\n\n";
?>
--EXPECT--
found
notfound
found
found

found
found
found
notfound

found
notfound
found
found

notfound
notfound
found
notfound
