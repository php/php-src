--TEST--
Bug #45682 (Unable to var_dump(DateInterval))
--INI--
date.timezone=UTC
--FILE--
<?php

$date = new DateTime("28-July-2008");
$other = new DateTime("31-July-2008");

$diff = date_diff($date, $other);

var_dump($diff);
--EXPECT--
object(DateInterval)#3 (8) {
  [u"y"]=>
  int(0)
  [u"m"]=>
  int(0)
  [u"d"]=>
  int(3)
  [u"h"]=>
  int(0)
  [u"i"]=>
  int(0)
  [u"s"]=>
  int(0)
  [u"invert"]=>
  int(0)
  [u"days"]=>
  int(3)
}
