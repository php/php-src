--TEST--
Testing clone on objects whose class derived from DateTimeZone class
--FILE--
<?php
//Set the default time zone
date_default_timezone_set("Europe/London");

class DateTimeZoneExt1 extends DateTimeZone {
    public $property1 = 99;
    public $property2 = "Hello";
}

class DateTimeZoneExt2 extends DateTimeZoneExt1 {
    public $property3 = true;
    public $property4 = 10.5;
}

echo "*** Testing clone on objects whose class derived from DateTimeZone class ***\n";

$d1 = new DateTimeZoneExt1("Europe/London");
var_dump($d1);
$d1_clone = clone $d1;
var_dump($d1_clone);

$d2 = new DateTimeZoneExt2("Europe/London");
var_dump($d2);
$d2_clone = clone $d2;
var_dump($d2_clone);

?>
--EXPECTF--
*** Testing clone on objects whose class derived from DateTimeZone class ***
object(DateTimeZoneExt1)#%d (4) {
  ["property1"]=>
  int(99)
  ["property2"]=>
  string(5) "Hello"
  ["timezone_type"]=>
  int(3)
  ["timezone"]=>
  string(13) "Europe/London"
}
object(DateTimeZoneExt1)#%d (4) {
  ["property1"]=>
  int(99)
  ["property2"]=>
  string(5) "Hello"
  ["timezone_type"]=>
  int(3)
  ["timezone"]=>
  string(13) "Europe/London"
}
object(DateTimeZoneExt2)#%d (6) {
  ["property3"]=>
  bool(true)
  ["property4"]=>
  float(10.5)
  ["property1"]=>
  int(99)
  ["property2"]=>
  string(5) "Hello"
  ["timezone_type"]=>
  int(3)
  ["timezone"]=>
  string(13) "Europe/London"
}
object(DateTimeZoneExt2)#%d (6) {
  ["property3"]=>
  bool(true)
  ["property4"]=>
  float(10.5)
  ["property1"]=>
  int(99)
  ["property2"]=>
  string(5) "Hello"
  ["timezone_type"]=>
  int(3)
  ["timezone"]=>
  string(13) "Europe/London"
}
