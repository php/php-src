--TEST--
GH-22993: DateInterval::createFromDateString() rejects embedded NUL byte
--FILE--
<?php

try {
    DateInterval::createFromDateString("foo\0bar");
} catch(Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}

?>
--EXPECTF--
ValueError: DateInterval::createFromDateString(): Argument #1 ($datetime) must not contain any null bytes
