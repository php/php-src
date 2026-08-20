--TEST--
GH-22993: DateTime::modify() error message with embedded NUL byte
--FILE--
<?php

$now = new DateTime();

try {
    $now->modify("foo\0bar");
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}

?>
--EXPECTF--
DateMalformedStringException: DateTime::modify(): Failed to parse time string (foo%0bar) at position %s
