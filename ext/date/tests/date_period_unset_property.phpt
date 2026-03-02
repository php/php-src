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
    echo $e->getMessage(), "\n";
}

try {
    unset($period->start);
} catch (Error $e) {
    echo $e->getMessage(), "\n";
}

try {
    unset($period->current);
} catch (Error $e) {
    echo $e->getMessage(), "\n";
}

try {
    unset($period->end);
} catch (Error $e) {
    echo $e->getMessage(), "\n";
}

try {
    unset($period->interval);
} catch (Error $e) {
    echo $e->getMessage(), "\n";
}

try {
    unset($period->recurrences);
} catch (Error $e) {
    echo $e->getMessage(), "\n";
}

try {
    unset($period->include_start_date);
} catch (Error $e) {
    echo $e->getMessage(), "\n";
}

try {
    unset($period->include_end_date);
} catch (Error $e) {
    echo $e->getMessage(), "\n";
}

?>
--EXPECT--
Typed property MyDatePeriod::$prop must not be accessed before initialization
Cannot unset MyDatePeriod::$start
Cannot unset MyDatePeriod::$current
Cannot unset MyDatePeriod::$end
Cannot unset MyDatePeriod::$interval
Cannot unset MyDatePeriod::$recurrences
Cannot unset MyDatePeriod::$include_start_date
Cannot unset MyDatePeriod::$include_end_date
