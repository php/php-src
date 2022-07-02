--TEST--
mail() with null bytes in arguments
--FILE--
<?php

try {
    mail("foo\0bar", "x", "y");
} catch (ValueError $e) {
    echo $e->getMessage(), "\n";
}
try {
    mail("x", "foo\0bar", "y");
} catch (ValueError $e) {
    echo $e->getMessage(), "\n";
}
try {
    mail("x", "y", "foo\0bar");
} catch (ValueError $e) {
    echo $e->getMessage(), "\n";
}
try {
    mail("x", "y", "z", "foo\0bar");
} catch (ValueError $e) {
    echo $e->getMessage(), "\n";
}
try {
    mail("x", "y", "z", "q", "foo\0bar");
} catch (ValueError $e) {
    echo $e->getMessage(), "\n";
}

?>
--EXPECT--
mail(): Argument #1 ($to) must not contain any null bytes
mail(): Argument #2 ($subject) must not contain any null bytes
mail(): Argument #3 ($message) must not contain any null bytes
mail(): Argument #4 ($additional_headers) must not contain any null bytes
mail(): Argument #5 ($additional_params) must not contain any null bytes
