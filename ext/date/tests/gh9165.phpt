--TEST--
GH-9165 (strtotime translates a date-time with DST/non-DST hour differently based on default timezone)
--FILE--
<?php
date_default_timezone_set('America/Lower_Princes');
echo strtotime("2018-10-28 01:00:00 Europe/London") . PHP_EOL;

date_default_timezone_set('Europe/London');
echo strtotime("2018-10-28 01:00:00 Europe/London"). PHP_EOL;

date_default_timezone_set('America/Lower_Princes');
echo strtotime("2018-10-28 04:00:00 Europe/London") . PHP_EOL;

date_default_timezone_set('Europe/London');
echo strtotime("2018-10-28 04:00:00 Europe/London"). PHP_EOL;
?>
--EXPECT--
1540684800
1540684800
1540699200
1540699200
