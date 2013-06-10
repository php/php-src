--TEST--
Bug #49081 (DateTime::diff() mistake if start in January and interval > 28 days)
--FILE--
<?php
   date_default_timezone_set('Europe/Berlin');
   $d1 = new DateTime('2010-01-01 06:00:00');
   $d2 = new DateTime('2010-01-31 10:00:00');
   $d  = $d1->diff($d2);
   print_r($d);
?>
--EXPECT--
DateInterval Object
(
    [y] => 0
    [m] => 0
    [d] => 30
    [h] => 4
    [i] => 0
    [s] => 0
    [weekday] => 0
    [weekday_behavior] => 0
    [first_last_day_of] => 0
    [invert] => 0
    [days] => 30
    [special_type] => 0
    [special_amount] => 0
    [have_weekday_relative] => 0
    [have_special_relative] => 0
)
