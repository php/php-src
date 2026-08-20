--TEST--
GH-22993: DatePeriod::createFromISO8601String() rejects embedded NUL byte
--FILE--
<?php

try {
    DatePeriod::createFromISO8601String("foo\0bar");
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}

?>
--EXPECT--
ValueError: DatePeriod::createFromISO8601String(): Argument #1 ($specification) must not contain any null bytes
