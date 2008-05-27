--TEST--
getdate() tests
--INI--
date.timezone=UTC
--FILE--
<?php

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
  [u"seconds"]=>
  int(0)
  [u"minutes"]=>
  int(0)
  [u"hours"]=>
  int(0)
  [u"mday"]=>
  int(27)
  [u"wday"]=>
  int(2)
  [u"mon"]=>
  int(6)
  [u"year"]=>
  int(2006)
  [u"yday"]=>
  int(177)
  [u"weekday"]=>
  string(7) "Tuesday"
  [u"month"]=>
  string(4) "June"
  [0]=>
  int(1151366400)
}
array(11) {
  [u"seconds"]=>
  int(%d)
  [u"minutes"]=>
  int(%d)
  [u"hours"]=>
  int(%d)
  [u"mday"]=>
  int(%d)
  [u"wday"]=>
  int(%d)
  [u"mon"]=>
  int(%d)
  [u"year"]=>
  int(%d)
  [u"yday"]=>
  int(%d)
  [u"weekday"]=>
  string(%d) "%s"
  [u"month"]=>
  string(%d) "%s"
  [0]=>
  int(%d)
}
Done
