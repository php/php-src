--TEST--
Bug #54283 (new DatePeriod(NULL) causes crash)
--FILE--
<?php

try {
    var_dump(new DatePeriod(NULL));
} catch (Exception $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}

?>
--EXPECTF--
Deprecated: DatePeriod::__construct(): Passing null to parameter #1 ($start) of type string is deprecated in %s on line %d

Deprecated: Calling DatePeriod::__construct(string $isostr, int $options = 0) is deprecated, use DatePeriod::createFromISO8601String() instead in %s on line %d
DateMalformedPeriodStringException: Unknown or bad format ()
