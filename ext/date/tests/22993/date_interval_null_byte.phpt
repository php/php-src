--TEST--
GH-22993: date_interval_create_from_date_string() rejects embedded NUL byte
--FILE--
<?php

try {
    date_interval_create_from_date_string("foo\0bar");
} catch(Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}

?>
--EXPECTF--
ValueError: date_interval_create_from_date_string(): Argument #1 ($datetime) must not contain any null bytes
