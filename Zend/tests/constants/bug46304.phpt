--TEST--
Bug #46304 (defining namespaced constant using define())
--FILE--
<?php

define('NS1\ns2\const1','value1');
define('ns1\ns2\const2','value2');
define('ns1\NS2\coNSt3','value3');

print NS1\ns2\const1 . "\n";
print ns1\ns2\const1 . "\n";
print ns1\NS2\const1 . "\n";

print NS1\ns2\const2 . "\n";
print ns1\ns2\const2 . "\n";
print ns1\NS2\const2 . "\n";

print NS1\ns2\coNSt3 . "\n";
print ns1\ns2\coNSt3 . "\n";
print ns1\ns2\coNSt3 . "\n";

print NS1\ns2\coNSt1 . "\n";
?>
--EXPECTF--
value1
value1
value1
value2
value2
value2
value3
value3
value3

Fatal error: Uncaught Error: Undefined constant "NS1\ns2\coNSt1" in %s:%d
Stack trace:
#0 {main}
  thrown in %sbug46304.php on line %d
