--TEST--
Test for date_parse_by_format()
--FILE--
<?php
$date = "06/08/04";
print_r( date_parse_from_format( '!m/d/y', $date ) );
print_r( date_parse_from_format( '!m*d*y', $date ) );
?>
--EXPECT--
Array
(
    [year] => 2004
    [month] => 6
    [day] => 8
    [hour] => 0
    [minute] => 0
    [second] => 0
    [fraction] => 0
    [warning_count] => 0
    [warnings] => Array
        (
        )

    [error_count] => 0
    [errors] => Array
        (
        )

    [is_localtime] => 
)
Array
(
    [year] => 2004
    [month] => 6
    [day] => 8
    [hour] => 0
    [minute] => 0
    [second] => 0
    [fraction] => 0
    [warning_count] => 0
    [warnings] => Array
        (
        )

    [error_count] => 0
    [errors] => Array
        (
        )

    [is_localtime] => 
)
