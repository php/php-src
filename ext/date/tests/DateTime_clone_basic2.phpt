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
  [u"property1"]=>
  int(99)
  [u"property2"]=>
  unicode(5) "Hello"
  [u"date"]=>
  unicode(19) "2009-02-03 12:34:41"
  [u"timezone_type"]=>
  int(2)
  [u"timezone"]=>
  unicode(3) "GMT"
}
object(DateTimeExt1)#%d (5) {
  [u"property1"]=>
  int(99)
  [u"property2"]=>
  unicode(5) "Hello"
  [u"date"]=>
  unicode(19) "2009-02-03 12:34:41"
  [u"timezone_type"]=>
  int(2)
  [u"timezone"]=>
  unicode(3) "GMT"
}
object(DateTimeExt2)#%d (7) {
  [u"property3"]=>
  bool(true)
  [u"property4"]=>
  float(10.5)
  [u"property1"]=>
  int(99)
  [u"property2"]=>
  unicode(5) "Hello"
  [u"date"]=>
  unicode(19) "2009-02-03 12:34:41"
  [u"timezone_type"]=>
  int(2)
  [u"timezone"]=>
  unicode(3) "GMT"
}
object(DateTimeExt2)#%d (7) {
  [u"property3"]=>
  bool(true)
  [u"property4"]=>
  float(10.5)
  [u"property1"]=>
  int(99)
  [u"property2"]=>
  unicode(5) "Hello"
  [u"date"]=>
  unicode(19) "2009-02-03 12:34:41"
  [u"timezone_type"]=>
  int(2)
  [u"timezone"]=>
  unicode(3) "GMT"
}
===DONE===
