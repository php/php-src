--TEST--
mail() with null bytes in arguments
--FILE--
<?php

try {
    mail("foo\0bar", "x", "y");
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}
try {
    mail("x", "foo\0bar", "y");
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}
try {
    mail("x", "y", "foo\0bar");
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}
try {
    mail("x", "y", "z", "foo\0bar");
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}
try {
    mail("x", "y", "z", "q", "foo\0bar");
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}

?>
--EXPECT--
ValueError: mail(): Argument #1 ($to) must not contain any null bytes
ValueError: mail(): Argument #2 ($subject) must not contain any null bytes
ValueError: mail(): Argument #3 ($message) must not contain any null bytes
ValueError: mail(): Argument #4 ($additional_headers) must not contain any null bytes
ValueError: mail(): Argument #5 ($additional_params) must not contain any null bytes
