--TEST--
GH-20936 (DatePeriod::__set_state() cannot handle null start)
--FILE--
<?php
$end = new DateTime('2022-07-16', new DateTimeZone('UTC'));
$interval = new DateInterval('P2D');
try {
    DatePeriod::__set_state(['start' => null, 'end' => $end, 'current' => null, 'interval' => $interval, 'recurrences' => 2, 'include_start_date' => false, 'include_end_date' => true]);
} catch (Throwable $e) {
    echo $e::class, ": ", $e->getMessage(), "\n";
}
?>
--EXPECT--
Error: Invalid serialization data for DatePeriod object
