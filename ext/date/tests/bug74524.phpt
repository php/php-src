--TEST--
Bug #74524 (Date diff is bad calculated, in same time zone)
--INI--
date.timezone=Europe/Amsterdam
--FILE--
<?php
$a = new DateTime("2017-11-17 22:05:26.000000");
$b = new DateTime("2017-04-03 22:29:15.079459");

$diff = $a->diff($b);
var_dump($diff);
?>
--EXPECTF--
object(DateTimeInterval)#%d (%d) {
  ["from_string"]=>
  bool(false)
  ["y"]=>
  int(0)
  ["m"]=>
  int(7)
  ["d"]=>
  int(13)
  ["h"]=>
  int(23)
  ["i"]=>
  int(36)
  ["s"]=>
  int(10)
  ["f"]=>
  float(0.920541)
  ["invert"]=>
  int(1)
  ["days"]=>
  int(227)
}
