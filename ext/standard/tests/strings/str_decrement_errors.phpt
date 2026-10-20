--TEST--
str_decrement(): Invalid strings to decrement should throw a ValueError
--FILE--
<?php

$strings = [
    // Empty string
    "",
    // String increments are unaware of being "negative"
    "-cc",
    // Trailing whitespace
    "Z ",
    // Leading whitespace
    " Z",
    // Non-ASCII characters
    "é",
    "あいうえお",
    "α",
    "ω",
    "Α", // Capital alpha
    "Ω",
    // With period
    "foo1.txt",
    "1f.5",
    // With multiple period
    "foo.1.txt",
    "1.f.5",
];

foreach ($strings as $s) {
    try {
        var_dump(str_decrement($s));
    } catch (Throwable $e) {
        echo $e::class, ': ', $e->getMessage(), "\n";
    }
}

?>
--EXPECT--
ValueError: str_decrement(): Argument #1 ($string) must not be empty
ValueError: str_decrement(): Argument #1 ($string) must be composed only of alphanumeric ASCII characters
ValueError: str_decrement(): Argument #1 ($string) must be composed only of alphanumeric ASCII characters
ValueError: str_decrement(): Argument #1 ($string) must be composed only of alphanumeric ASCII characters
ValueError: str_decrement(): Argument #1 ($string) must be composed only of alphanumeric ASCII characters
ValueError: str_decrement(): Argument #1 ($string) must be composed only of alphanumeric ASCII characters
ValueError: str_decrement(): Argument #1 ($string) must be composed only of alphanumeric ASCII characters
ValueError: str_decrement(): Argument #1 ($string) must be composed only of alphanumeric ASCII characters
ValueError: str_decrement(): Argument #1 ($string) must be composed only of alphanumeric ASCII characters
ValueError: str_decrement(): Argument #1 ($string) must be composed only of alphanumeric ASCII characters
ValueError: str_decrement(): Argument #1 ($string) must be composed only of alphanumeric ASCII characters
ValueError: str_decrement(): Argument #1 ($string) must be composed only of alphanumeric ASCII characters
ValueError: str_decrement(): Argument #1 ($string) must be composed only of alphanumeric ASCII characters
ValueError: str_decrement(): Argument #1 ($string) must be composed only of alphanumeric ASCII characters
