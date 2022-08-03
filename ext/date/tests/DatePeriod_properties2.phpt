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
        echo $e->getMessage() . "\n";
    }

    try {
        $period->$property[] = "extra";
    } catch (Error $e) {
        echo $e->getMessage() . "\n";
    }
}

try {
    $period->start->modify("+1 hour");
} catch (Error $e) {
    echo $e->getMessage() . "\n";
}

?>
--EXPECT--
Cannot modify readonly property DatePeriod::$recurrences
Cannot modify readonly property DatePeriod::$recurrences
Cannot modify readonly property DatePeriod::$include_start_date
Cannot modify readonly property DatePeriod::$include_start_date
Cannot modify readonly property DatePeriod::$start
Cannot modify readonly property DatePeriod::$start
Cannot modify readonly property DatePeriod::$current
Cannot modify readonly property DatePeriod::$current
Cannot modify readonly property DatePeriod::$end
Cannot modify readonly property DatePeriod::$end
Cannot modify readonly property DatePeriod::$interval
Cannot modify readonly property DatePeriod::$interval
