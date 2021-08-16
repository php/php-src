--TEST--
Bug #80998 Missing second with inverted interval
--FILE--
<?php
$date = new DateTime('2000-01-01 00:00:00');
$interval = new DateInterval('PT1S');
$interval->f = 0.5;
$interval->invert = 1;
$date->add($interval);

echo $date->format('Y-m-d H:i:s.u'), "\n";
?>
--EXPECT--
1999-12-31 23:59:58.500000
