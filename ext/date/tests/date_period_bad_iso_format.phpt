--TEST--
Test bad ISO date formats passed to DatePeriod constructor
--FILE--
<?php

try {
    new DatePeriod("R4");
} catch (Exception $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}

try {
    DatePeriod::createFromISO8601String("R4");
} catch (Exception $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}

try {
    new DatePeriod("R4/2012-07-01T00:00:00Z");
} catch (Exception $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}

try {
    DatePeriod::createFromISO8601String("R4/2012-07-01T00:00:00Z");
} catch (Exception $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}

try {
    new DatePeriod("2012-07-01T00:00:00Z/P7D");
} catch (Exception $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}

try {
    DatePeriod::createFromISO8601String("2012-07-01T00:00:00Z/P7D");
} catch (Exception $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}

?>
--EXPECT--
DateMalformedPeriodStringException: DatePeriod::__construct(): ISO interval must contain a start date, "R4" given
DateMalformedPeriodStringException: DatePeriod::createFromISO8601String(): ISO interval must contain a start date, "R4" given
DateMalformedPeriodStringException: DatePeriod::__construct(): ISO interval must contain an interval, "R4/2012-07-01T00:00:00Z" given
DateMalformedPeriodStringException: DatePeriod::createFromISO8601String(): ISO interval must contain an interval, "R4/2012-07-01T00:00:00Z" given
DateMalformedPeriodStringException: DatePeriod::__construct(): Recurrence count must be greater than 0
DateMalformedPeriodStringException: DatePeriod::createFromISO8601String(): Recurrence count must be greater than 0
