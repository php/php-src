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
===DONE===
--EXPECTF--
*** Testing clone on DateTime objects ***

-- Create a DateTimeZone object --
object(DateTimeZone)#%d (0) {
}

-- Add some properties --
object(DateTimeZone)#%d (2) {
  ["property1"]=>
  int(99)
  ["property2"]=>
  string(5) "Hello"
}

-- clone it --
object(DateTimeZone)#%d (2) {
  ["property1"]=>
  int(99)
  ["property2"]=>
  string(5) "Hello"
}

-- Add some more properties --
object(DateTimeZone)#%d (4) {
  ["property1"]=>
  int(99)
  ["property2"]=>
  string(5) "Hello"
  ["property3"]=>
  bool(true)
  ["property4"]=>
  float(10.5)
}

-- clone it --
object(DateTimeZone)#%d (4) {
  ["property1"]=>
  int(99)
  ["property2"]=>
  string(5) "Hello"
  ["property3"]=>
  bool(true)
  ["property4"]=>
  float(10.5)
}
===DONE===
