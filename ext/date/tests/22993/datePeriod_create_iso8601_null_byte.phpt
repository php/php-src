--TEST--
GH-22993: DatePeriod::createFromISO8601String() error message with embedded NUL byte
--FILE--
<?php

try {
    DatePeriod::createFromISO8601String("foo\0bar");
} catch (DateMalformedPeriodStringException $e) {
    echo $e->getMessage(), PHP_EOL;
}

?>
--EXPECTF--
Unknown or bad format (foo%0bar)

