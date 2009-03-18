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
  [u"name"]=>
  unicode(8) "DateTime"
}
..and get names of all its methods
array(18) {
  [0]=>
  &object(ReflectionMethod)#%d (2) {
    [u"name"]=>
    unicode(11) "__construct"
    [u"class"]=>
    unicode(8) "DateTime"
  }
  [1]=>
  &object(ReflectionMethod)#%d (2) {
    [u"name"]=>
    unicode(8) "__wakeup"
    [u"class"]=>
    unicode(8) "DateTime"
  }
  [2]=>
  &object(ReflectionMethod)#%d (2) {
    [u"name"]=>
    unicode(11) "__set_state"
    [u"class"]=>
    unicode(8) "DateTime"
  }
  [3]=>
  &object(ReflectionMethod)#%d (2) {
    [u"name"]=>
    unicode(16) "createFromFormat"
    [u"class"]=>
    unicode(8) "DateTime"
  }
  [4]=>
  &object(ReflectionMethod)#%d (2) {
    [u"name"]=>
    unicode(13) "getLastErrors"
    [u"class"]=>
    unicode(8) "DateTime"
  }
  [5]=>
  &object(ReflectionMethod)#%d (2) {
    [u"name"]=>
    unicode(6) "format"
    [u"class"]=>
    unicode(8) "DateTime"
  }
  [6]=>
  &object(ReflectionMethod)#%d (2) {
    [u"name"]=>
    unicode(6) "modify"
    [u"class"]=>
    unicode(8) "DateTime"
  }
  [7]=>
  &object(ReflectionMethod)#%d (2) {
    [u"name"]=>
    unicode(3) "add"
    [u"class"]=>
    unicode(8) "DateTime"
  }
  [8]=>
  &object(ReflectionMethod)#%d (2) {
    [u"name"]=>
    unicode(3) "sub"
    [u"class"]=>
    unicode(8) "DateTime"
  }
  [9]=>
  &object(ReflectionMethod)#%d (2) {
    [u"name"]=>
    unicode(11) "getTimezone"
    [u"class"]=>
    unicode(8) "DateTime"
  }
  [10]=>
  &object(ReflectionMethod)#%d (2) {
    [u"name"]=>
    unicode(11) "setTimezone"
    [u"class"]=>
    unicode(8) "DateTime"
  }
  [11]=>
  &object(ReflectionMethod)#%d (2) {
    [u"name"]=>
    unicode(9) "getOffset"
    [u"class"]=>
    unicode(8) "DateTime"
  }
  [12]=>
  &object(ReflectionMethod)#%d (2) {
    [u"name"]=>
    unicode(7) "setTime"
    [u"class"]=>
    unicode(8) "DateTime"
  }
  [13]=>
  &object(ReflectionMethod)#%d (2) {
    [u"name"]=>
    unicode(7) "setDate"
    [u"class"]=>
    unicode(8) "DateTime"
  }
  [14]=>
  &object(ReflectionMethod)#%d (2) {
    [u"name"]=>
    unicode(10) "setISODate"
    [u"class"]=>
    unicode(8) "DateTime"
  }
  [15]=>
  &object(ReflectionMethod)#%d (2) {
    [u"name"]=>
    unicode(12) "setTimestamp"
    [u"class"]=>
    unicode(8) "DateTime"
  }
  [16]=>
  &object(ReflectionMethod)#%d (2) {
    [u"name"]=>
    unicode(12) "getTimestamp"
    [u"class"]=>
    unicode(8) "DateTime"
  }
  [17]=>
  &object(ReflectionMethod)#%d (2) {
    [u"name"]=>
    unicode(4) "diff"
    [u"class"]=>
    unicode(8) "DateTime"
  }
}
..and get names of all its class constants
array(11) {
  [u"ATOM"]=>
  unicode(13) "Y-m-d\TH:i:sP"
  [u"COOKIE"]=>
  unicode(16) "l, d-M-y H:i:s T"
  [u"ISO8601"]=>
  unicode(13) "Y-m-d\TH:i:sO"
  [u"RFC822"]=>
  unicode(16) "D, d M y H:i:s O"
  [u"RFC850"]=>
  unicode(16) "l, d-M-y H:i:s T"
  [u"RFC1036"]=>
  unicode(16) "D, d M y H:i:s O"
  [u"RFC1123"]=>
  unicode(16) "D, d M Y H:i:s O"
  [u"RFC2822"]=>
  unicode(16) "D, d M Y H:i:s O"
  [u"RFC3339"]=>
  unicode(13) "Y-m-d\TH:i:sP"
  [u"RSS"]=>
  unicode(16) "D, d M Y H:i:s O"
  [u"W3C"]=>
  unicode(13) "Y-m-d\TH:i:sP"
}
===DONE===
