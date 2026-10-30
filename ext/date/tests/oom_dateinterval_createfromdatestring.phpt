--TEST--
DateInterval::createFromDateString() doesn't reach hard limit
--INI--
memory_limit=64M
--FILE--
<?php
try {
    var_dump(DateInterval::createFromDateString(str_repeat("\x01", 4 * 1024 * 1024)));
} catch (\Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}
?>
--EXPECTF--
DateMalformedIntervalStringException: Unknown or bad format (%s) at position 0 (%s): Unexpected character
