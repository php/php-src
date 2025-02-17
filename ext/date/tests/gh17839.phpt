--TEST--
Bug GH-17839 Negative SSE + fractions results in wrong timestamp
--FILE--
<?php

$dt1 = new DateTime('@1.1');
$dt2 = new DateTime('@-1.1');
var_dump($dt1, $dt1->getTimestamp());
var_dump($dt2, $dt2->getTimestamp());
--EXPECTF--
object(DateTime)#%d (%d) {
  ["date"]=>
  string(26) "1970-01-01 00:00:01.100000"
  ["timezone_type"]=>
  int(1)
  ["timezone"]=>
  string(6) "+00:00"
}
int(1)
object(DateTime)#%d (%d) {
  ["date"]=>
  string(26) "1969-12-31 23:59:58.900000"
  ["timezone_type"]=>
  int(1)
  ["timezone"]=>
  string(6) "+00:00"
}
int(-1)
