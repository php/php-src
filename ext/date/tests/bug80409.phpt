--TEST--
Bug #80409: DateTime::modify() loses time with 'weekday' parameter
--INI--
date.timezone=UTC
--FILE--
<?php
$date = '2020-11-27 12:33:00';

echo date('Y-m-d H:i:s', strtotime($date.' +1 day')), "\n";
echo date('Y-m-d H:i:s', strtotime($date.' +1 Weekday')), "\n";

$date1 = new DateTime($date);
$date2 = new DateTime($date);

$date2->modify('+1 day');
echo $date2->format('Y-m-d H:i:s'), "\n";

$date1->modify('+1 weekday');
echo $date1->format('Y-m-d H:i:s'), "\n";
?>
--EXPECT--
2020-11-28 12:33:00
2020-11-30 12:33:00
2020-11-28 12:33:00
2020-11-30 12:33:00
