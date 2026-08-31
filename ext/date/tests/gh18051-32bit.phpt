--TEST--
Bug GH-18051 (Interger overflow on 32bit)
--SKIPIF--
<?php if (PHP_INT_SIZE != 4) die("skip 32-bit only");
--FILE--
<?php

$tz = new DateTimeZone('America/New_York');
var_dump(array_slice($tz->getTransitions(), 0, 3));

--EXPECT--
array(3) {
  [0]=>
  array(5) {
    ["ts"]=>
    int(-2147483648)
    ["time"]=>
    string(25) "1901-12-13T20:45:52+00:00"
    ["offset"]=>
    int(-18000)
    ["isdst"]=>
    bool(false)
    ["abbr"]=>
    string(3) "EST"
  }
  [1]=>
  array(5) {
    ["ts"]=>
    int(-1633280400)
    ["time"]=>
    string(25) "1918-03-31T07:00:00+00:00"
    ["offset"]=>
    int(-14400)
    ["isdst"]=>
    bool(true)
    ["abbr"]=>
    string(3) "EDT"
  }
  [2]=>
  array(5) {
    ["ts"]=>
    int(-1615140000)
    ["time"]=>
    string(25) "1918-10-27T06:00:00+00:00"
    ["offset"]=>
    int(-18000)
    ["isdst"]=>
    bool(false)
    ["abbr"]=>
    string(3) "EST"
  }
}
