--TEST--
Test clone of objects whoose class derived from DateTime class
--FILE--
<?php
//Set the default time zone 
date_default_timezone_set("Europe/London");

class DateTimeExt1 extends DateTime {
	public $property1 = 99;
	public $property2 = "Hello";

}

class DateTimeExt2 extends DateTimeExt1 {
	public $property3 = true;
	public $property4 = 10.5;
}

echo "*** Testing clone on objects whoose class derived from DateTime class ***\n";

$d1 = new DateTimeExt1("2009-02-03 12:34:41 GMT");
var_dump($d1);
$d1_clone = clone $d1;
var_dump($d1_clone);

$d2 = new DateTimeExt2("2009-02-03 12:34:41 GMT");
var_dump($d2);
$d2_clone = clone $d2;
var_dump($d2_clone);
?>
===DONE===
--EXPECTF--
*** Testing clone on objects whoose class derived from DateTime class ***
object(DateTimeExt1)#%d (5) {
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
object(DateTimeExt1)#%d (5) {
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
object(DateTimeExt2)#%d (7) {
  ["property3"]=>
  bool(true)
  ["property4"]=>
  float(10.5)
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
object(DateTimeExt2)#%d (7) {
  ["property3"]=>
  bool(true)
  ["property4"]=>
  float(10.5)
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
===DONE===
