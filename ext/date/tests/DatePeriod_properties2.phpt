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

?>
--EXPECT--
Writing to DatePeriod properties is unsupported
Retrieval of DatePeriod properties for modification is unsupported
Writing to DatePeriod properties is unsupported
Retrieval of DatePeriod properties for modification is unsupported
Writing to DatePeriod properties is unsupported
Retrieval of DatePeriod properties for modification is unsupported
Writing to DatePeriod properties is unsupported
Retrieval of DatePeriod properties for modification is unsupported
Writing to DatePeriod properties is unsupported
Retrieval of DatePeriod properties for modification is unsupported
Writing to DatePeriod properties is unsupported
Retrieval of DatePeriod properties for modification is unsupported
