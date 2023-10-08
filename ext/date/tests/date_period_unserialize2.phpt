--TEST--
Test that calling DatePeriod::__unserialize() directly with missing arguments throws
--FILE--
<?php

$start = new DateTime("2022-07-14 00:00:00", new DateTimeZone("UTC"));
$interval = new DateInterval('P1D');
$end = new DateTime("2022-07-16 00:00:00", new DateTimeZone("UTC"));
$period = new DatePeriod($start, $interval, $end);

try {
    $period->__unserialize(
        [
            "start" => new DateTime,
        ]
    );
} catch (\Error $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}

?>
--EXPECT--
Error: Invalid serialization data for DatePeriod object
