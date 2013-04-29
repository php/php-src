--TEST--
Test for + character in date format
--FILE--
<?php
$date = "06/08/04 12:00";
print_r( date_parse_from_format( 'm/d/y', $date ) );
print_r( date_parse_from_format( 'm/d/y+', $date ) );
print_r( date_parse_from_format( '+m/d/y', $date ) );
print_r( date_parse_from_format( 'm/d/y++', $date ) );

$date = "06/08/04";
print_r( date_parse_from_format( 'm/d/y+', $date ) );
print_r( date_parse_from_format( '+m/d/y', $date ) );

?>
--EXPECT--
Array
(
    [year] => 2004
    [month] => 6
    [day] => 8
    [hour] => 
    [minute] => 
    [second] => 
    [fraction] => 
    [warning_count] => 0
    [warnings] => Array
        (
        )

    [error_count] => 1
    [errors] => Array
        (
            [8] => Trailing data
        )

    [is_localtime] => 
)
Array
(
    [year] => 2004
    [month] => 6
    [day] => 8
    [hour] => 
    [minute] => 
    [second] => 
    [fraction] => 
    [warning_count] => 1
    [warnings] => Array
        (
            [8] => Trailing data
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
    [hour] => 
    [minute] => 
    [second] => 
    [fraction] => 
    [warning_count] => 1
    [warnings] => Array
        (
            [8] => Trailing data
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
    [hour] => 
    [minute] => 
    [second] => 
    [fraction] => 
    [warning_count] => 1
    [warnings] => Array
        (
            [8] => Trailing data
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
    [hour] => 
    [minute] => 
    [second] => 
    [fraction] => 
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
    [hour] => 
    [minute] => 
    [second] => 
    [fraction] => 
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
