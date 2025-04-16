--TEST--
Bug GH-18051 (DateTimeZone->getTransitions can return first transition twice)
--FILE--
<?php

$tzBln = new DateTimeZone('Europe/Berlin');
var_dump($tzBln->getTransitions(922582799, 941331599));
var_dump($tzBln->getTransitions(922582800, 941331600));
var_dump($tzBln->getTransitions(922582801, 941331601));

--EXPECT--
array(2) {
  [0]=>
  array(5) {
    ["ts"]=>
    int(922582799)
    ["time"]=>
    string(25) "1999-03-28T00:59:59+00:00"
    ["offset"]=>
    int(3600)
    ["isdst"]=>
    bool(false)
    ["abbr"]=>
    string(3) "CET"
  }
  [1]=>
  array(5) {
    ["ts"]=>
    int(922582800)
    ["time"]=>
    string(25) "1999-03-28T01:00:00+00:00"
    ["offset"]=>
    int(7200)
    ["isdst"]=>
    bool(true)
    ["abbr"]=>
    string(4) "CEST"
  }
}
array(2) {
  [0]=>
  array(5) {
    ["ts"]=>
    int(922582800)
    ["time"]=>
    string(25) "1999-03-28T01:00:00+00:00"
    ["offset"]=>
    int(7200)
    ["isdst"]=>
    bool(true)
    ["abbr"]=>
    string(4) "CEST"
  }
  [1]=>
  array(5) {
    ["ts"]=>
    int(941331600)
    ["time"]=>
    string(25) "1999-10-31T01:00:00+00:00"
    ["offset"]=>
    int(3600)
    ["isdst"]=>
    bool(false)
    ["abbr"]=>
    string(3) "CET"
  }
}
array(2) {
  [0]=>
  array(5) {
    ["ts"]=>
    int(922582801)
    ["time"]=>
    string(25) "1999-03-28T01:00:01+00:00"
    ["offset"]=>
    int(7200)
    ["isdst"]=>
    bool(true)
    ["abbr"]=>
    string(4) "CEST"
  }
  [1]=>
  array(5) {
    ["ts"]=>
    int(941331600)
    ["time"]=>
    string(25) "1999-10-31T01:00:00+00:00"
    ["offset"]=>
    int(3600)
    ["isdst"]=>
    bool(false)
    ["abbr"]=>
    string(3) "CET"
  }
}
