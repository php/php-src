--TEST--
DateTime::modify() doesn't reach hard limit
--INI--
memory_limit=64M
--FILE--
<?php
try {
    var_dump((new DateTime())->modify(str_repeat("\x01", 4 * 1024 * 1024)));
} catch (\Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}
?>
--EXPECTF--
DateMalformedStringException: DateTime::modify(): Failed to parse time string (%s) at position 0 (%s): Unexpected character
