--TEST--
GH-22993: DateTimeImmutable rejects embedded NUL byte
--FILE--
<?php

try {
    new DateTimeImmutable("foo\0bar");
} catch(Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}

?>
--EXPECT--
ValueError: DateTimeImmutable::__construct(): Argument #1 ($datetime) must not contain any null bytes
