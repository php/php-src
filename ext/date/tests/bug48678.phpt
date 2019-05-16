--TEST--
Bug #48678 (DateInterval segfaults when unserialising)
--FILE--
<?php
$x = new DateInterval("P3Y6M4DT12H30M5S");
print_r($x);
$y = unserialize(serialize($x));
print_r($y);
--EXPECT--
DateInterval Object
(
    [y] => 3
    [m] => 6
    [d] => 4
    [h] => 12
    [i] => 30
    [s] => 5
    [f] => 0
    [weekday] => 0
    [weekday_behavior] => 0
    [first_last_day_of] => 0
    [invert] => 0
    [days] => 
    [special_type] => 0
    [special_amount] => 0
    [have_weekday_relative] => 0
    [have_special_relative] => 0
)
DateInterval Object
(
    [y] => 3
    [m] => 6
    [d] => 4
    [h] => 12
    [i] => 30
    [s] => 5
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
