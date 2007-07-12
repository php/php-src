--TEST--
Bug #41709 (strtotime() does not handle 00.00.0000)
--FILE--
<?php
date_default_timezone_set("UTC");

$date_string = '00.00.0000 - 00:00:00';
print_r(date_parse($date_string));

?>
--EXPECT--
Array
(
    [year] => 0
    [month] => 0
    [day] => 0
    [hour] => 0
    [minute] => 0
    [second] => 0
    [fraction] => 0
    [warning_count] => 0
    [warnings] => Array
        (
        )

    [error_count] => 1
    [errors] => Array
        (
            [11] => Unexpected character
        )

    [is_localtime] => 
)
