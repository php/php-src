--TEST--
GH-22993: DateInterval error message with embedded NUL byte
--FILE--
<?php

try {
    new DateInterval("foo\0bar");
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}

?>
--EXPECTF--
ValueError: DateInterval::__construct(): Argument #1 ($duration) must not contain any null bytes
