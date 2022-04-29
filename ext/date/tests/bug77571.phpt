--TEST--
Bug #77571 (DateTime's diff DateInterval incorrect in timezones from UTC+01:00 to UTC+12:00)
--FILE--
<?php

date_default_timezone_set('Europe/London');

$date3 = DateTime::createFromFormat('Y-m-d H:i:s', '2019-04-01 00:00:00'); //  2019-04-01 00:00:00.0 Europe/London (+01:00)
$date4 = clone $date3;
$date4->modify('+5 week'); // 2019-05-06 00:00:00.0 Europe/London (+01:00)
$differenceDateInterval2 = $date3->diff($date4); // interval: + 1m 4d; days 35
print_r($differenceDateInterval2);
?>
--EXPECT--
DateInterval Object
(
    [y] => 0
    [m] => 1
    [d] => 5
    [h] => 0
    [i] => 0
    [s] => 0
    [f] => 0
    [invert] => 0
    [days] => 35
    [from_string] => 
)
