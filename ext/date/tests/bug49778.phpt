--TEST--
Bug #49778 (DateInterval::format("%a") is always zero when an interval is created from an ISO string)
--FILE--
<?php
$i=new DateInterval('P7D');
var_dump($i);
echo $i->format("%d"), "\n";
echo $i->format("%a"), "\n";
?>
--EXPECTF--
object(DateInterval)#%d (%d) {
  ["from_string"]=>
  bool(false)
  ["y"]=>
  int(0)
  ["m"]=>
  int(0)
  ["d"]=>
  int(7)
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
}
7
(unknown)
