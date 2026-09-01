--TEST--
GH-22993: date_default_timezone_set() rejects embedded NUL byte
--FILE--
<?php

try {
    date_default_timezone_set("foo\0bar");
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}

?>
--EXPECT--
ValueError: date_default_timezone_set(): Argument #1 ($timezoneId) must not contain any null bytes
