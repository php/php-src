--TEST--
Bug #74274 (Handling DST transitions correctly)
--FILE--
<?php
$tz = new DateTimeZone("Europe/Paris");
$startDate = new \DateTime('2018-10-28 00:00:00', $tz);
$endDateBuggy = new \DateTime('2018-10-29 23:00:00', $tz);

print_r($startDate->diff($endDateBuggy));
?>
--EXPECT--
DateInterval Object
(
    [y] => 0
    [m] => 0
    [d] => 1
    [h] => 23
    [i] => 0
    [s] => 0
    [f] => 0
    [weekday] => 0
    [weekday_behavior] => 0
    [first_last_day_of] => 0
    [invert] => 0
    [days] => 1
    [special_type] => 0
    [special_amount] => 0
    [have_weekday_relative] => 0
    [have_special_relative] => 0
)
