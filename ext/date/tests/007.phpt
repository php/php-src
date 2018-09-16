--TEST--
localtime() tests
--FILE--
<?php
date_default_timezone_set('UTC');

$t = mktime(0,0,0, 6, 27, 2006);
var_dump(localtime(1,1,1));

var_dump(localtime());
var_dump(localtime($t));
var_dump(localtime($t, true));
var_dump(localtime($t, false));

echo "Done\n";
?>
--EXPECTF--
Warning: localtime() expects at most 2 parameters, 3 given in %s on line %d
bool(false)
array(9) {
  [0]=>
  int(%d)
  [1]=>
  int(%d)
  [2]=>
  int(%d)
  [3]=>
  int(%d)
  [4]=>
  int(%d)
  [5]=>
  int(%d)
  [6]=>
  int(%d)
  [7]=>
  int(%d)
  [8]=>
  int(%d)
}
array(9) {
  [0]=>
  int(0)
  [1]=>
  int(0)
  [2]=>
  int(0)
  [3]=>
  int(27)
  [4]=>
  int(5)
  [5]=>
  int(106)
  [6]=>
  int(2)
  [7]=>
  int(177)
  [8]=>
  int(0)
}
array(9) {
  ["tm_sec"]=>
  int(0)
  ["tm_min"]=>
  int(0)
  ["tm_hour"]=>
  int(0)
  ["tm_mday"]=>
  int(27)
  ["tm_mon"]=>
  int(5)
  ["tm_year"]=>
  int(106)
  ["tm_wday"]=>
  int(2)
  ["tm_yday"]=>
  int(177)
  ["tm_isdst"]=>
  int(0)
}
array(9) {
  [0]=>
  int(0)
  [1]=>
  int(0)
  [2]=>
  int(0)
  [3]=>
  int(27)
  [4]=>
  int(5)
  [5]=>
  int(106)
  [6]=>
  int(2)
  [7]=>
  int(177)
  [8]=>
  int(0)
}
Done
