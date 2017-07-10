--TEST--
Bug #74529 DateTime::diff seems to ignore $absolute = true
--FILE--
<?php

date_default_timezone_set('Europe/Amsterdam');

$date1 = date_create("2014-11-01 00:00:00");
$date2 = date_create("2017-05-01 04:00:00");

$diff1 = $date2->diff($date1, true);
$diff2 = $date1->diff($date2, true);

echo sprintf('%s %s %s %s', $diff1->m, $diff1->d, $diff1->h, $diff1->i) . "\n";
echo sprintf('%s %s %s %s', $diff2->m, $diff2->d, $diff2->h, $diff2->i);
?>
--EXPECT--
6 0 4 0
6 0 4 0
