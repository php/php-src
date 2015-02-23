--TEST--
Test DateTime class registration
--FILE--
<?php

echo "*** Verify DateTime class ***\n";

echo "Verify DateTime class registered OK\n";
$class = new ReflectionClass('DateTime');
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
*** Verify DateTime class ***
Verify DateTime class registered OK
object(ReflectionClass)#%d (1) {
  ["name"]=>
  string(8) "DateTime"
}
..and get names of all its methods
array(18) {
  [0]=>
  object(ReflectionMethod)#%d (2) {
    ["name"]=>
    string(11) "__construct"
    ["class"]=>
    string(8) "DateTime"
  }
  [1]=>
  object(ReflectionMethod)#%d (2) {
    ["name"]=>
    string(8) "__wakeup"
    ["class"]=>
    string(8) "DateTime"
  }
  [2]=>
  object(ReflectionMethod)#%d (2) {
    ["name"]=>
    string(11) "__set_state"
    ["class"]=>
    string(8) "DateTime"
  }
  [3]=>
  object(ReflectionMethod)#%d (2) {
    ["name"]=>
    string(16) "createFromFormat"
    ["class"]=>
    string(8) "DateTime"
  }
  [4]=>
  object(ReflectionMethod)#%d (2) {
    ["name"]=>
    string(13) "getLastErrors"
    ["class"]=>
    string(8) "DateTime"
  }
  [5]=>
  object(ReflectionMethod)#%d (2) {
    ["name"]=>
    string(6) "format"
    ["class"]=>
    string(8) "DateTime"
  }
  [6]=>
  object(ReflectionMethod)#%d (2) {
    ["name"]=>
    string(6) "modify"
    ["class"]=>
    string(8) "DateTime"
  }
  [7]=>
  object(ReflectionMethod)#%d (2) {
    ["name"]=>
    string(3) "add"
    ["class"]=>
    string(8) "DateTime"
  }
  [8]=>
  object(ReflectionMethod)#%d (2) {
    ["name"]=>
    string(3) "sub"
    ["class"]=>
    string(8) "DateTime"
  }
  [9]=>
  object(ReflectionMethod)#%d (2) {
    ["name"]=>
    string(11) "getTimezone"
    ["class"]=>
    string(8) "DateTime"
  }
  [10]=>
  object(ReflectionMethod)#%d (2) {
    ["name"]=>
    string(11) "setTimezone"
    ["class"]=>
    string(8) "DateTime"
  }
  [11]=>
  object(ReflectionMethod)#%d (2) {
    ["name"]=>
    string(9) "getOffset"
    ["class"]=>
    string(8) "DateTime"
  }
  [12]=>
  object(ReflectionMethod)#%d (2) {
    ["name"]=>
    string(7) "setTime"
    ["class"]=>
    string(8) "DateTime"
  }
  [13]=>
  object(ReflectionMethod)#%d (2) {
    ["name"]=>
    string(7) "setDate"
    ["class"]=>
    string(8) "DateTime"
  }
  [14]=>
  object(ReflectionMethod)#%d (2) {
    ["name"]=>
    string(10) "setISODate"
    ["class"]=>
    string(8) "DateTime"
  }
  [15]=>
  object(ReflectionMethod)#%d (2) {
    ["name"]=>
    string(12) "setTimestamp"
    ["class"]=>
    string(8) "DateTime"
  }
  [16]=>
  object(ReflectionMethod)#%d (2) {
    ["name"]=>
    string(12) "getTimestamp"
    ["class"]=>
    string(8) "DateTime"
  }
  [17]=>
  object(ReflectionMethod)#%d (2) {
    ["name"]=>
    string(4) "diff"
    ["class"]=>
    string(8) "DateTime"
  }
}
..and get names of all its class constants
array(12) {
  ["ATOM"]=>
  string(13) "Y-m-d\TH:i:sP"
  ["COOKIE"]=>
  string(16) "l, d-M-Y H:i:s T"
  ["ISO8601"]=>
  string(13) "Y-m-d\TH:i:sO"
  ["RFC822"]=>
  string(16) "D, d M y H:i:s O"
  ["RFC850"]=>
  string(16) "l, d-M-y H:i:s T"
  ["RFC1036"]=>
  string(16) "D, d M y H:i:s O"
  ["RFC1123"]=>
  string(16) "D, d M Y H:i:s O"
  ["RFC2822"]=>
  string(16) "D, d M Y H:i:s O"
  ["RFC3339"]=>
  string(13) "Y-m-d\TH:i:sP"
  ["RFC3339_EXTENDED"]=>
  string(15) "Y-m-d\TH:i:s.vP"
  ["RSS"]=>
  string(16) "D, d M Y H:i:s O"
  ["W3C"]=>
  string(13) "Y-m-d\TH:i:sP"
}
===DONE===
