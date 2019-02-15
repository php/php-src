--TEST--
getdate() tests
--FILE--
<?php
date_default_timezone_set('UTC');

$t = mktime(0,0,0, 6, 27, 2006);
var_dump(getdate(1,1));

var_dump(getdate($t));
var_dump(getdate());

echo "Done\n";
?>
--EXPECTF--
Warning: getdate() expects at most 1 parameter, 2 given in %s on line %d
bool(false)
array(11) {
  ["seconds"]=>
  int(0)
  ["minutes"]=>
  int(0)
  ["hours"]=>
  int(0)
  ["mday"]=>
  int(27)
  ["wday"]=>
  int(2)
  ["mon"]=>
  int(6)
  ["year"]=>
  int(2006)
  ["yday"]=>
  int(177)
  ["weekday"]=>
  string(7) "Tuesday"
  ["month"]=>
  string(4) "June"
  [0]=>
  int(1151366400)
}
array(11) {
  ["seconds"]=>
  int(%d)
  ["minutes"]=>
  int(%d)
  ["hours"]=>
  int(%d)
  ["mday"]=>
  int(%d)
  ["wday"]=>
  int(%d)
  ["mon"]=>
  int(%d)
  ["year"]=>
  int(%d)
  ["yday"]=>
  int(%d)
  ["weekday"]=>
  string(%d) "%s"
  ["month"]=>
  string(%d) "%s"
  [0]=>
  int(%d)
}
Done
