--TEST--
DatePeriod: Test cannot modify read only properties
--INI--
date.timezone=UTC
--FILE--
<?php

$period = new DatePeriod(new DateTime, new DateInterval('P1D'), new DateTime);

$properties = [
    "recurrences",
    "include_start_date",
    "start",
    "current",
    "end",
    "interval",
];

foreach ($properties as $property) {
    try {
        $period->$property = "new";
    } catch (Error $e) {
        echo $e::class, ': ', $e->getMessage(), "\n";
    }

    try {
        $period->$property[] = "extra";
    } catch (Error $e) {
        echo $e::class, ': ', $e->getMessage(), "\n";
    }
}

try {
    $period->start->modify("+1 hour");
} catch (Error $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}

?>
--EXPECT--
Error: Cannot modify readonly property DatePeriod::$recurrences
Error: Cannot modify readonly property DatePeriod::$recurrences
Error: Cannot modify readonly property DatePeriod::$include_start_date
Error: Cannot modify readonly property DatePeriod::$include_start_date
Error: Cannot modify readonly property DatePeriod::$start
Error: Cannot modify readonly property DatePeriod::$start
Error: Cannot modify readonly property DatePeriod::$current
Error: Cannot modify readonly property DatePeriod::$current
Error: Cannot modify readonly property DatePeriod::$end
Error: Cannot modify readonly property DatePeriod::$end
Error: Cannot modify readonly property DatePeriod::$interval
Error: Cannot modify readonly property DatePeriod::$interval
