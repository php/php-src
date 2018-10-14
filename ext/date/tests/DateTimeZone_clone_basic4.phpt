--TEST--
Test clone of DateTimeZone derived objects with __clone magic method
--FILE--
<?php
//Set the default time zone
date_default_timezone_set("Europe/London");

class DateTimeZoneExt1 extends DateTimeZone {
	public function __clone() {
		echo "-- DateTimeExt1 __clone magic method called --\n";
	}
}

echo "*** Testing clone of objects derived from DateTimeZone class with __clone magic method***\n";

$d1 = new DateTimeZoneExt1("America/New_York");
$d1_clone = clone $d1;

//verify clone by calling method on new object
var_dump( $d1_clone->getName() );

?>
===DONE===
--EXPECTF--
*** Testing clone of objects derived from DateTimeZone class with __clone magic method***
-- DateTimeExt1 __clone magic method called --
string(16) "America/New_York"
===DONE===
