--TEST--
DatePeriod::__construct() doesn't reach hard limit
--INI--
memory_limit=64M
--SKIPIF--
<?php
if (getenv('SKIP_ASAN')) die("skip ASan-instrumented realloc() is slow");
?>
--FILE--
<?php
try {
    new DatePeriod(str_repeat("\x01", 4 * 1024 * 1024));
} catch (\Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}
?>
--EXPECTF--
Deprecated: Calling DatePeriod::__construct(string $isostr, int $options = 0) is deprecated, use DatePeriod::createFromISO8601String() instead in %s on line %d
DateMalformedPeriodStringException: Unknown or bad format (%s)
