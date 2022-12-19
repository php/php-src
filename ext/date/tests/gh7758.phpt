--TEST--
Bug GH-7758 (Problems with negative timestamps and fractions)
--FILE--
<?php
date_default_timezone_set('UTC');

foreach ([0.4, 0, -0.4, -1, -1.4] as $ts) {
    $date = new DateTime('@' . $ts);
    print_r($date);
}
?>
--EXPECT--
DateTime Object
(
    [date] => 1970-01-01 00:00:00.400000
    [timezone_type] => 1
    [timezone] => +00:00
)
DateTime Object
(
    [date] => 1970-01-01 00:00:00.000000
    [timezone_type] => 1
    [timezone] => +00:00
)
DateTime Object
(
    [date] => 1969-12-31 23:59:59.600000
    [timezone_type] => 1
    [timezone] => +00:00
)
DateTime Object
(
    [date] => 1969-12-31 23:59:59.000000
    [timezone_type] => 1
    [timezone] => +00:00
)
DateTime Object
(
    [date] => 1969-12-31 23:59:58.600000
    [timezone_type] => 1
    [timezone] => +00:00
)
