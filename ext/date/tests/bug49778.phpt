--TEST--
Bug #49778 (DateInterval::format("%a") is always zero when an interval is created from an ISO string)
--FILE--
<?php
$i=new DateInterval('P7D');
var_dump($i);
echo $i->format("%d"), "\n";
echo $i->format("%a"), "\n";
?>
--EXPECT--
object(DateInterval)#1 (8) {
  [u"y"]=>
  int(0)
  [u"m"]=>
  int(0)
  [u"d"]=>
  int(7)
  [u"h"]=>
  int(0)
  [u"i"]=>
  int(0)
  [u"s"]=>
  int(0)
  [u"invert"]=>
  int(0)
  [u"days"]=>
  bool(false)
}
7
(unknown)
