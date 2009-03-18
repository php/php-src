--TEST--
Test clone of DateTime objects
--FILE--
<?php
//Set the default time zone 
date_default_timezone_set("Europe/London");

echo "*** Testing clone on DateTime objects ***\n";

echo "\n-- Create a DateTime object --\n";
$d1 = new DateTime("2009-02-03 12:34:41 GMT");
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

-- Create a DateTime object --
object(DateTime)#%d (0) {
}

-- Add some properties --
object(DateTime)#%d (2) {
  ["property1"]=>
  int(99)
  ["property2"]=>
  string(5) "Hello"
}

-- clone it --
object(DateTime)#%d (2) {
  ["property1"]=>
  int(99)
  ["property2"]=>
  string(5) "Hello"
}

-- Add some more properties --
object(DateTime)#%d (4) {
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
object(DateTime)#%d (4) {
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
