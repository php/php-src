--TEST--
Test unsetting DatePeriod properties
--FILE--
<?php

class MyDatePeriod extends DatePeriod {
    public int $prop = 3;
}

$period = new MyDatePeriod(new DateTimeImmutable("now"), DateInterval::createFromDateString("tomorrow"), 1);

unset($period->prop);

try {
    $period->prop;
} catch (Error $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}

try {
    unset($period->start);
} catch (Error $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}

try {
    unset($period->current);
} catch (Error $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}

try {
    unset($period->end);
} catch (Error $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}

try {
    unset($period->interval);
} catch (Error $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}

try {
    unset($period->recurrences);
} catch (Error $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}

try {
    unset($period->include_start_date);
} catch (Error $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}

try {
    unset($period->include_end_date);
} catch (Error $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}

?>
--EXPECT--
Error: Typed property MyDatePeriod::$prop must not be accessed before initialization
Error: Cannot unset MyDatePeriod::$start
Error: Cannot unset MyDatePeriod::$current
Error: Cannot unset MyDatePeriod::$end
Error: Cannot unset MyDatePeriod::$interval
Error: Cannot unset MyDatePeriod::$recurrences
Error: Cannot unset MyDatePeriod::$include_start_date
Error: Cannot unset MyDatePeriod::$include_end_date
