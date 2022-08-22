--TEST--
Bug GH-8730 (DateTime::diff miscalculation is same time zone of different type)
--FILE--
<?php
$foo = new DateTime('2022-06-08 09:15:00', new DateTimeZone('-04:00'));
$bar = new DateTime('2022-06-08 09:15:00', new DateTimeZone('US/Eastern'));
print_r($foo->diff($bar));
?>
--EXPECT--
DateInterval Object
(
    [y] => 0
    [m] => 0
    [d] => 0
    [h] => 0
    [i] => 0
    [s] => 0
    [f] => 0
    [weekday] => 0
    [weekday_behavior] => 0
    [first_last_day_of] => 0
    [invert] => 0
    [days] => 0
    [special_type] => 0
    [special_amount] => 0
    [have_weekday_relative] => 0
    [have_special_relative] => 0
)
