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
Writing to DatePeriod->recurrences is unsupported
Retrieval of DatePeriod->recurrences for modification is unsupported
Writing to DatePeriod->include_start_date is unsupported
Retrieval of DatePeriod->include_start_date for modification is unsupported
Writing to DatePeriod->start is unsupported
Retrieval of DatePeriod->start for modification is unsupported
Writing to DatePeriod->current is unsupported
Retrieval of DatePeriod->current for modification is unsupported
Writing to DatePeriod->end is unsupported
Retrieval of DatePeriod->end for modification is unsupported
Writing to DatePeriod->interval is unsupported
Retrieval of DatePeriod->interval for modification is unsupported
