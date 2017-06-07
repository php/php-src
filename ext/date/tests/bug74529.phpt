--TEST--
Bug #74529 DateTime::diff seems to ignore $absolute = true
--FILE--
<?php

$date1 = date_create("2014-11-01 00:00:00");
$date2 = date_create("2017-05-01 04:00:00");

$diff1 = $date2->diff($date1, true);
$diff2 = $date1->diff($date2, true);

echo $diff1->m .' months, '.$diff1->d. " days\n";
echo $diff2->m .' months, '.$diff2->d. ' days';
?>
--EXPECT--
6 months, 0 days
6 months, 0 days
