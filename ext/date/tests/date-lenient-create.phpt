--TEST--
Test for + character in date format
--FILE--
<?php
$tz = new DateTimeZone("UTC");
$date = "06/08/04 12:00";
echo "==\n";
print_r( date_create_from_format( 'm/d/y', $date , $tz) );
print_r( date_get_last_errors() );
echo "==\n";
print_r( date_create_from_format( 'm/d/y+', $date , $tz)->setTime(0, 0) );
print_r( date_get_last_errors() );
echo "==\n";
print_r( date_create_from_format( '+m/d/y', $date , $tz)->setTime(0, 0) );
print_r( date_get_last_errors() );
echo "==\n";
print_r( date_create_from_format( 'm/d/y++', $date , $tz)->setTime(0, 0) );
print_r( date_get_last_errors() );
echo "==\n";

$date = "06/08/04";
print_r( date_create_from_format( 'm/d/y+', $date , $tz)->setTime(0, 0) );
print_r( date_get_last_errors() );
echo "==\n";
print_r( date_create_from_format( '+m/d/y', $date , $tz)->setTime(0, 0) );
print_r( date_get_last_errors() );
echo "==\n";

?>
--EXPECT--
==
Array
(
    [warning_count] => 0
    [warnings] => Array
        (
        )

    [error_count] => 1
    [errors] => Array
        (
            [8] => Trailing data
        )

)
==
DateTime Object
(
    [date] => 2004-06-08 00:00:00.000000
    [timezone_type] => 3
    [timezone] => UTC
)
Array
(
    [warning_count] => 1
    [warnings] => Array
        (
            [8] => Trailing data
        )

    [error_count] => 0
    [errors] => Array
        (
        )

)
==
DateTime Object
(
    [date] => 2004-06-08 00:00:00.000000
    [timezone_type] => 3
    [timezone] => UTC
)
Array
(
    [warning_count] => 1
    [warnings] => Array
        (
            [8] => Trailing data
        )

    [error_count] => 0
    [errors] => Array
        (
        )

)
==
DateTime Object
(
    [date] => 2004-06-08 00:00:00.000000
    [timezone_type] => 3
    [timezone] => UTC
)
Array
(
    [warning_count] => 1
    [warnings] => Array
        (
            [8] => Trailing data
        )

    [error_count] => 0
    [errors] => Array
        (
        )

)
==
DateTime Object
(
    [date] => 2004-06-08 00:00:00.000000
    [timezone_type] => 3
    [timezone] => UTC
)
Array
(
    [warning_count] => 0
    [warnings] => Array
        (
        )

    [error_count] => 0
    [errors] => Array
        (
        )

)
==
DateTime Object
(
    [date] => 2004-06-08 00:00:00.000000
    [timezone_type] => 3
    [timezone] => UTC
)
Array
(
    [warning_count] => 0
    [warnings] => Array
        (
        )

    [error_count] => 0
    [errors] => Array
        (
        )

)
==
