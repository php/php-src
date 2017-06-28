--TEST--
Bug #65657 (Use of a timezone other than 'Z' causes parser to fail)
--INI--
--FILE--
<?php
$iso = '2013-09-09T12:00:15+02:00';
$datetime = new DateTime($iso);
print_r($datetime);
$iso = '2013-09-09T12:00:15Z';
$datetime = new DateTime($iso);
print_r($datetime);
$iso = 'R2/2013-09-09T12:00:15+02:00/P1D';
$periods = new DatePeriod($iso);
print_r($periods->start);
$iso = 'R2/2013-09-09T12:00:15Z/P1D';
$periods = new DatePeriod($iso);
print_r($periods->start);
?>
--EXPECT--
DateTime Object
(
    [date] => 2013-09-09 12:00:15.000000
    [timezone_type] => 1
    [timezone] => +02:00
)
DateTime Object
(
    [date] => 2013-09-09 12:00:15.000000
    [timezone_type] => 2
    [timezone] => Z
)
DateTime Object
(
    [date] => 2013-09-09 12:00:15.000000
    [timezone_type] => 1
    [timezone] => +02:00
)
DateTime Object
(
    [date] => 2013-09-09 12:00:15.000000
    [timezone_type] => 2
    [timezone] => Z
)
