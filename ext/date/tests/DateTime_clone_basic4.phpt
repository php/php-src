--TEST--
Test clone of DateTime derived objects with __clone magic method
--FILE--
<?php
//Set the default time zone 
date_default_timezone_set("Europe/London");

//Set the default time zone 
date_default_timezone_set("Europe/London");

class DateTimeExt1 extends DateTime {
	public function __clone() {
		echo "-- DateTimeExt1 __clone magic method called --\n"; 
	}
}

echo "*** Testing clone of objects derived from DateTime class with __clone magic method***\n";

$d1 = new DateTimeExt1("2009-02-03 12:34:41 GMT");
$d1_clone = clone $d1;

//verify clone by calling method on new object
var_dump( $d1_clone->format( "m.d.y") ); 

?>
===DONE===
--EXPECTF--
*** Testing clone of objects derived from DateTime class with __clone magic method***
-- DateTimeExt1 __clone magic method called --
string(8) "02.03.09"
===DONE===
