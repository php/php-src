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
  ["name"]=>
  string(12) "DateTimeZone"
}
..and get names of all its methods
array(9) {
  [0]=>
  &object(ReflectionMethod)#%d (2) {
    ["name"]=>
    string(11) "__construct"
    ["class"]=>
    string(12) "DateTimeZone"
  }
  [1]=>
  &object(ReflectionMethod)#%d (2) {
    ["name"]=>
    string(8) "__wakeup"
    ["class"]=>
    string(12) "DateTimeZone"
  }
  [2]=>
  &object(ReflectionMethod)#%d (2) {
    ["name"]=>
    string(11) "__set_state"
    ["class"]=>
    string(12) "DateTimeZone"
  }
  [3]=>
  &object(ReflectionMethod)#%d (2) {
    ["name"]=>
    string(7) "getName"
    ["class"]=>
    string(12) "DateTimeZone"
  }
  [4]=>
  &object(ReflectionMethod)#%d (2) {
    ["name"]=>
    string(9) "getOffset"
    ["class"]=>
    string(12) "DateTimeZone"
  }
  [5]=>
  &object(ReflectionMethod)#%d (2) {
    ["name"]=>
    string(14) "getTransitions"
    ["class"]=>
    string(12) "DateTimeZone"
  }
  [6]=>
  &object(ReflectionMethod)#%d (2) {
    ["name"]=>
    string(11) "getLocation"
    ["class"]=>
    string(12) "DateTimeZone"
  }
  [7]=>
  &object(ReflectionMethod)#%d (2) {
    ["name"]=>
    string(17) "listAbbreviations"
    ["class"]=>
    string(12) "DateTimeZone"
  }
  [8]=>
  &object(ReflectionMethod)#%d (2) {
    ["name"]=>
    string(15) "listIdentifiers"
    ["class"]=>
    string(12) "DateTimeZone"
  }
}
..and get names of all its class constants
array(14) {
  ["AFRICA"]=>
  int(1)
  ["AMERICA"]=>
  int(2)
  ["ANTARCTICA"]=>
  int(4)
  ["ARCTIC"]=>
  int(8)
  ["ASIA"]=>
  int(16)
  ["ATLANTIC"]=>
  int(32)
  ["AUSTRALIA"]=>
  int(64)
  ["EUROPE"]=>
  int(128)
  ["INDIAN"]=>
  int(256)
  ["PACIFIC"]=>
  int(512)
  ["UTC"]=>
  int(1024)
  ["ALL"]=>
  int(2047)
  ["ALL_WITH_BC"]=>
  int(4095)
  ["PER_COUNTRY"]=>
  int(4096)
}
===DONE===
