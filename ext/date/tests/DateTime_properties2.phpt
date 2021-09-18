--TEST--
DateTime: Test cannot modify read only properties
--FILE--
<?php

$period = new DateTime();

$properties = [
    "year",
    "month",
    "day",
    "hour",
    "minute",
    "second",
    "microsecond",
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
Writing to DateTime->year is unsupported
Retrieval of DateTime->year for modification is unsupported
Writing to DateTime->month is unsupported
Retrieval of DateTime->month for modification is unsupported
Writing to DateTime->day is unsupported
Retrieval of DateTime->day for modification is unsupported
Writing to DateTime->hour is unsupported
Retrieval of DateTime->hour for modification is unsupported
Writing to DateTime->minute is unsupported
Retrieval of DateTime->minute for modification is unsupported
Writing to DateTime->second is unsupported
Retrieval of DateTime->second for modification is unsupported
Writing to DateTime->microsecond is unsupported
Retrieval of DateTime->microsecond for modification is unsupported
