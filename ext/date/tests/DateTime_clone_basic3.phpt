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
--EXPECTF--
*** Testing clone on DateTime objects ***

-- Create a DateTime object --
object(DateTime)#%d (3) {
  ["date"]=>
  string(26) "2009-02-03 12:34:41.000000"
  ["timezone_type"]=>
  int(2)
  ["timezone"]=>
  string(3) "GMT"
}

-- Add some properties --
object(DateTime)#%d (5) {
  ["property1"]=>
  int(99)
  ["property2"]=>
  string(5) "Hello"
  ["date"]=>
  string(26) "2009-02-03 12:34:41.000000"
  ["timezone_type"]=>
  int(2)
  ["timezone"]=>
  string(3) "GMT"
}

-- clone it --
object(DateTime)#%d (5) {
  ["property1"]=>
  int(99)
  ["property2"]=>
  string(5) "Hello"
  ["date"]=>
  string(26) "2009-02-03 12:34:41.000000"
  ["timezone_type"]=>
  int(2)
  ["timezone"]=>
  string(3) "GMT"
}

-- Add some more properties --
object(DateTime)#%d (7) {
  ["property1"]=>
  int(99)
  ["property2"]=>
  string(5) "Hello"
  ["property3"]=>
  bool(true)
  ["property4"]=>
  float(10.5)
  ["date"]=>
  string(26) "2009-02-03 12:34:41.000000"
  ["timezone_type"]=>
  int(2)
  ["timezone"]=>
  string(3) "GMT"
}

-- clone it --
object(DateTime)#%d (7) {
  ["property1"]=>
  int(99)
  ["property2"]=>
  string(5) "Hello"
  ["property3"]=>
  bool(true)
  ["property4"]=>
  float(10.5)
  ["date"]=>
  string(26) "2009-02-03 12:34:41.000000"
  ["timezone_type"]=>
  int(2)
  ["timezone"]=>
  string(3) "GMT"
}
