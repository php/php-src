--TEST--
Test DateTimeZone class registration
--FILE--
<?php

echo "*** Verify DateTimeZone class ***\n";

echo "Verify DateTimeZone class registered OK\n";
$class = new ReflectionClass('DateTimeZone');
var_dump($class);

echo "..and get names of all its methods\n";
$methods = $class->getMethods(); 
var_dump($methods);

echo "..and get names of all its class constants\n"; 
$constants = $class->getConstants();
var_dump($constants);
?>
===DONE===
--EXPECTF--
*** Verify DateTimeZone class ***
Verify DateTimeZone class registered OK
object(ReflectionClass)#%d (1) {
  [u"name"]=>
  unicode(12) "DateTimeZone"
}
..and get names of all its methods
array(7) {
  [0]=>
  &object(ReflectionMethod)#%d (2) {
    [u"name"]=>
    unicode(11) "__construct"
    [u"class"]=>
    unicode(12) "DateTimeZone"
  }
  [1]=>
  &object(ReflectionMethod)#%d (2) {
    [u"name"]=>
    unicode(7) "getName"
    [u"class"]=>
    unicode(12) "DateTimeZone"
  }
  [2]=>
  &object(ReflectionMethod)#%d (2) {
    [u"name"]=>
    unicode(9) "getOffset"
    [u"class"]=>
    unicode(12) "DateTimeZone"
  }
  [3]=>
  &object(ReflectionMethod)#%d (2) {
    [u"name"]=>
    unicode(14) "getTransitions"
    [u"class"]=>
    unicode(12) "DateTimeZone"
  }
  [4]=>
  &object(ReflectionMethod)#%d (2) {
    [u"name"]=>
    unicode(11) "getLocation"
    [u"class"]=>
    unicode(12) "DateTimeZone"
  }
  [5]=>
  &object(ReflectionMethod)#%d (2) {
    [u"name"]=>
    unicode(17) "listAbbreviations"
    [u"class"]=>
    unicode(12) "DateTimeZone"
  }
  [6]=>
  &object(ReflectionMethod)#%d (2) {
    [u"name"]=>
    unicode(15) "listIdentifiers"
    [u"class"]=>
    unicode(12) "DateTimeZone"
  }
}
..and get names of all its class constants
array(14) {
  [u"AFRICA"]=>
  int(1)
  [u"AMERICA"]=>
  int(2)
  [u"ANTARCTICA"]=>
  int(4)
  [u"ARCTIC"]=>
  int(8)
  [u"ASIA"]=>
  int(16)
  [u"ATLANTIC"]=>
  int(32)
  [u"AUSTRALIA"]=>
  int(64)
  [u"EUROPE"]=>
  int(128)
  [u"INDIAN"]=>
  int(256)
  [u"PACIFIC"]=>
  int(512)
  [u"UTC"]=>
  int(1024)
  [u"ALL"]=>
  int(2047)
  [u"ALL_WITH_BC"]=>
  int(4095)
  [u"PER_COUNTRY"]=>
  int(4096)
}
===DONE===