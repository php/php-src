--TEST--
Bug #65657 (Use of a timezone other than 'Z' causes parser to fail)
--INI--
--FILE--
<?php
$iso = 'R2/2013-09-09T12:00:15+02:00/P1D';
$periods = new DatePeriod($iso);
print_r($periods);
?>
--EXPECT--
DatePeriod Object
(
    [start] => DateTime Object
        (
            [date] => 2013-09-09 12:00:15.000000
            [timezone_type] => 1
            [timezone] => +02:00
        )

    [current] => 
    [end] => 
    [interval] => DateInterval Object
        (
            [y] => 0
            [m] => 0
            [d] => 1
            [h] => 0
            [i] => 0
            [s] => 0
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

    [recurrences] => 3
    [include_start_date] => 1
)
