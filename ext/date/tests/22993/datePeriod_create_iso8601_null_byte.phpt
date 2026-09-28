--TEST--
GH-22993: DatePeriod::createFromISO8601String() rejects embedded NUL byte
--FILE--
<?php

try {
    DatePeriod::createFromISO8601String("foo\0bar");
} catch (ValueError $e) {
    echo $e->getMessage(), PHP_EOL;
}

?>
--EXPECT--
DatePeriod::createFromISO8601String(): Argument #1 ($specification) must not contain any null bytes