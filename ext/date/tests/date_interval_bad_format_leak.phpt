--TEST--
DateInterval with bad format should not leak period
--FILE--
<?php

try {
    new DateInterval('P3"D');
} catch (Exception $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}

try {
    new DatePeriod('P3"D');
} catch (Exception $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}

try {
    DatePeriod::createFromISO8601String('P3"D');
} catch (Exception $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}

try {
    new DatePeriod('2008-03-01T12:00:00Z1');
} catch (Exception $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}

try {
    DatePeriod::createFromISO8601String('2008-03-01T12:00:00Z1');
} catch (Exception $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}

?>
--EXPECT--
DateMalformedIntervalStringException: Unknown or bad format (P3"D)
DateMalformedPeriodStringException: Unknown or bad format (P3"D)
DateMalformedPeriodStringException: Unknown or bad format (P3"D)
DateMalformedPeriodStringException: Unknown or bad format (2008-03-01T12:00:00Z1)
DateMalformedPeriodStringException: Unknown or bad format (2008-03-01T12:00:00Z1)
