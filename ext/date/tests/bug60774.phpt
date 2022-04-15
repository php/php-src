--TEST--
Bug #60774 (DateInterval::format("%a") is always zero when an interval is created using the createFromDateString method)
--FILE--
<?php
$i= DateInterval::createFromDateString('2 days');
var_dump($i);
echo $i->format("%d"), "\n";
echo $i->format("%a"), "\n";
?>
--EXPECTF--
object(DateInterval)#1 (%d) {
  ["y"]=>
  int(0)
  ["m"]=>
  int(0)
  ["d"]=>
  int(2)
  ["h"]=>
  int(0)
  ["i"]=>
  int(0)
  ["s"]=>
  int(0)
  ["f"]=>
  float(0)
  ["invert"]=>
  int(0)
  ["days"]=>
  bool(false)
  ["have_special_relative"]=>
  int(0)
}
2
(unknown)
