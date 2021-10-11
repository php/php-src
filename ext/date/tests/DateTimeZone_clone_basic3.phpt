--TEST--
Test clone of DateTimeZOne objects
--FILE--
<?php
//Set the default time zone
date_default_timezone_set("Europe/London");

echo "*** Testing clone on DateTime objects ***\n";

echo "\n-- Create a DateTimeZone object --\n";
$d1 = new DateTimeZone("Europe/London");
var_dump($d1);
echo "\n-- Add some properties --\n";
$d1->property1 = 99;
$d1->property2 = "Hello";
var_dump($d1);
echo "\n-- clone it --\n";
$d1_clone = clone $d1;
var_dump($d1_clone);
echo "\n-- Add some more properties --\n";
$d1_clone->property3 = true;
$d1_clone->property4 = 10.5;
var_dump($d1_clone);
echo "\n-- clone it --\n";
$d2_clone = clone $d1_clone;
var_dump($d2_clone);
?>
--EXPECTF--
*** Testing clone on DateTime objects ***

-- Create a DateTimeZone object --
object(DateTimeZone)#%d (2) {
  ["timezone_type"]=>
  int(3)
  ["timezone"]=>
  string(13) "Europe/London"
}

-- Add some properties --

Deprecated: Creation of dynamic property DateTimeZone::$property1 is deprecated in %s on line %d

Deprecated: Creation of dynamic property DateTimeZone::$property2 is deprecated in %s on line %d
object(DateTimeZone)#%d (4) {
  ["property1"]=>
  int(99)
  ["property2"]=>
  string(5) "Hello"
  ["timezone_type"]=>
  int(3)
  ["timezone"]=>
  string(13) "Europe/London"
}

-- clone it --
object(DateTimeZone)#%d (4) {
  ["property1"]=>
  int(99)
  ["property2"]=>
  string(5) "Hello"
  ["timezone_type"]=>
  int(3)
  ["timezone"]=>
  string(13) "Europe/London"
}

-- Add some more properties --

Deprecated: Creation of dynamic property DateTimeZone::$property3 is deprecated in %s on line %d

Deprecated: Creation of dynamic property DateTimeZone::$property4 is deprecated in %s on line %d
object(DateTimeZone)#%d (6) {
  ["property1"]=>
  int(99)
  ["property2"]=>
  string(5) "Hello"
  ["property3"]=>
  bool(true)
  ["property4"]=>
  float(10.5)
  ["timezone_type"]=>
  int(3)
  ["timezone"]=>
  string(13) "Europe/London"
}

-- clone it --
object(DateTimeZone)#%d (6) {
  ["property1"]=>
  int(99)
  ["property2"]=>
  string(5) "Hello"
  ["property3"]=>
  bool(true)
  ["property4"]=>
  float(10.5)
  ["timezone_type"]=>
  int(3)
  ["timezone"]=>
  string(13) "Europe/London"
}
