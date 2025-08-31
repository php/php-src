--TEST--
Test for setting Date/Time during a forward DST transition
--FILE--
<?php
date_default_timezone_set('America/Chicago');

$date = new DateTime('2020-03-08 01:30:00');
echo $date->setTime(2, 0)->format('Y-m-d H:i:s T/e - U') . "\n";

$date = new DateTime('2020-03-08 01:30:00');
echo $date->setTime(2, 30)->format('Y-m-d H:i:s T/e - U') . "\n";

$date = new DateTime('2020-03-08 01:30:00');
echo $date->setTime(3, 0)->format('Y-m-d H:i:s T/e - U') . "\n";

$date = new DateTime('2020-03-08 01:30:00');
echo $date->setTime(1, 59, 59)->format('Y-m-d H:i:s T/e - U') . "\n";

?>
--EXPECT--
2020-03-08 03:00:00 CDT/America/Chicago - 1583654400
2020-03-08 03:30:00 CDT/America/Chicago - 1583656200
2020-03-08 03:00:00 CDT/America/Chicago - 1583654400
2020-03-08 01:59:59 CST/America/Chicago - 1583654399
