--TEST--
Test of compare object handler for DateTime objects
--FILE--
<?php 

// NB: DateTimeZone class does not define a customized compare class handler so standard object handler will be used  

echo "Simple test for DateTimeZone compare object handler\n";

//Set the default time zone 
date_default_timezone_set("Europe/London");

class DateTimeZoneExt1 extends DateTimeZone {
}

class DateTimeZoneExt2 extends DateTimeZone{
	public $foo = "Hello";
	private $bar = 99;
}

class DateTimeZoneExt3 extends DateTimeZoneExt2 {
}

$obj1 = new DateTimeZone("Europe/London");
$obj2 = new DateTimeZoneExt1("Europe/London");
$obj3 = new DateTimeZoneExt2("Europe/London");
$obj4 = new DateTimeZoneExt3("Europe/London");

echo "\n-- All the following tests should compare equal --\n";
var_dump($obj1 == $obj1);
echo "\n-- All the following tests should compare NOT equal --\n";
var_dump($obj1 == $obj2);
var_dump($obj1 == $obj3);
var_dump($obj1 == $obj4);
var_dump($obj2 == $obj3);
var_dump($obj2 == $obj4);
var_dump($obj3 == $obj4);

?>
===DONE===
--EXPECT--
Simple test for DateTimeZone compare object handler

-- All the following tests should compare equal --
bool(true)

-- All the following tests should compare NOT equal --
bool(false)
bool(false)
bool(false)
bool(false)
bool(false)
bool(false)
===DONE===