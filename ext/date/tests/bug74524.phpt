--TEST--
Bug #74524 (Date diff is bad calculated, in same time zone)
--INI--
date.timezone=Europe/Amsterdam
--FILE--
<?php
$a = new DateTime("2017-11-17 22:05:26.000000");
$b = new DateTime("2017-04-03 22:29:15.079459");

$diff = $a->diff($b);
print_r($diff);
?>
--EXPECT--
DateInterval Object
(
    [y] => 0
    [m] => 7
    [d] => 13
    [h] => 23
    [i] => 36
    [s] => 10
    [f] => 0.920541
    [weekday] => 0
    [weekday_behavior] => 0
    [first_last_day_of] => 0
    [invert] => 1
    [days] => 227
    [special_type] => 0
    [special_amount] => 0
    [have_weekday_relative] => 0
    [have_special_relative] => 0
)
