--TEST--
DateInterval::__construct() doesn't reach hard limit
--INI--
memory_limit=64M
--SKIPIF--
<?php
if (getenv('SKIP_ASAN')) die("skip ASan-instrumented realloc() is slow");
?>
--FILE--
<?php
try {
    new DateInterval(str_repeat("\x01", 4 * 1024 * 1024));
} catch (\Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}
?>
--EXPECTF--
DateMalformedIntervalStringException: Unknown or bad format (%s)
