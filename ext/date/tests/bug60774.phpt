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
  ["from_string"]=>
  bool(true)
  ["date_string"]=>
  string(6) "2 days"
}
2
(unknown)
