--TEST--
mb_send_mail() with null bytes in arguments
--EXTENSIONS--
mbstring
--FILE--
<?php

try {
    mb_send_mail("foo\0bar", "x", "y");
} catch (ValueError $e) {
    echo $e->getMessage(), "\n";
}
try {
    mb_send_mail("x", "foo\0bar", "y");
} catch (ValueError $e) {
    echo $e->getMessage(), "\n";
}
try {
    mb_send_mail("x", "y", "foo\0bar");
} catch (ValueError $e) {
    echo $e->getMessage(), "\n";
}
try {
    mb_send_mail("x", "y", "z", "foo\0bar");
} catch (ValueError $e) {
    echo $e->getMessage(), "\n";
}
try {
    mb_send_mail("x", "y", "z", "q", "foo\0bar");
} catch (ValueError $e) {
    echo $e->getMessage(), "\n";
}

?>
--EXPECT--
mb_send_mail(): Argument #1 ($to) must not contain any null bytes
mb_send_mail(): Argument #2 ($subject) must not contain any null bytes
mb_send_mail(): Argument #3 ($message) must not contain any null bytes
mb_send_mail(): Argument #4 ($additional_headers) must not contain any null bytes
mb_send_mail(): Argument #5 ($additional_params) must not contain any null bytes
