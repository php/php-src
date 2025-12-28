--TEST--
DatePeriod: take microseconds into account
--FILE--
<?php
date_default_timezone_set('UTC');
$start = new DateTime('2010-06-07T01:02:03.456789');
$end = new DateTime('2010-06-10T01:02:03.456789');
$interval = new DateInterval('P1D');

echo "from " . $start->format('Y-m-d H:i:s.u') . " to " . $end->format('Y-m-d H:i:s.u') . " (exclusive)\n";
foreach (new DatePeriod($start, $interval, $end) as $day) {
    echo $day->format('Y-m-d H:i:s.u') . "\n";
}

echo "from " . $start->format('Y-m-d H:i:s.u') . " to " . $end->format('Y-m-d H:i:s.u') . " (inclusive)\n";
foreach (new DatePeriod($start, $interval, $end, DatePeriod::INCLUDE_END_DATE) as $day) {
    echo $day->format('Y-m-d H:i:s.u') . "\n";
}

$end = new DateTime('2010-06-10T01:02:03.456790');
echo "from " . $start->format('Y-m-d H:i:s.u') . " to " . $end->format('Y-m-d H:i:s.u') . " (exclusive)\n";
foreach (new DatePeriod($start, $interval, $end) as $day) {
    echo $day->format('Y-m-d H:i:s.u') . "\n";
}

$end = new DateTime('2010-06-10T01:02:03.456788');
echo "from " . $start->format('Y-m-d H:i:s.u') . " to " . $end->format('Y-m-d H:i:s.u') . " (inclusive)\n";
foreach (new DatePeriod($start, $interval, $end, DatePeriod::INCLUDE_END_DATE) as $day) {
    echo $day->format('Y-m-d H:i:s.u') . "\n";
}

?>
--EXPECT--
from 2010-06-07 01:02:03.456789 to 2010-06-10 01:02:03.456789 (exclusive)
2010-06-07 01:02:03.456789
2010-06-08 01:02:03.456789
2010-06-09 01:02:03.456789
from 2010-06-07 01:02:03.456789 to 2010-06-10 01:02:03.456789 (inclusive)
2010-06-07 01:02:03.456789
2010-06-08 01:02:03.456789
2010-06-09 01:02:03.456789
2010-06-10 01:02:03.456789
from 2010-06-07 01:02:03.456789 to 2010-06-10 01:02:03.456790 (exclusive)
2010-06-07 01:02:03.456789
2010-06-08 01:02:03.456789
2010-06-09 01:02:03.456789
2010-06-10 01:02:03.456789
from 2010-06-07 01:02:03.456789 to 2010-06-10 01:02:03.456788 (inclusive)
2010-06-07 01:02:03.456789
2010-06-08 01:02:03.456789
2010-06-09 01:02:03.456789
