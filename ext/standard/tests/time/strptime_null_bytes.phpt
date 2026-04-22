--TEST--
strptime() rejects null bytes
--SKIPIF--
<?php
if (!function_exists('strptime')) {
    die("skip - strptime() function not available in this build");
}
?>
--FILE--
<?php

try {
    strptime("2024-01-01\0UTC", "%Y-%m-%d");
} catch (ValueError $e) {
    echo $e->getMessage(), "\n";
}

try {
    strptime("2024-01-01", "%Y-%m-%d\0");
} catch (ValueError $e) {
    echo $e->getMessage(), "\n";
}

?>
--EXPECTF--
Deprecated: Function strptime() is deprecated since 8.2, use date_parse_from_format() (for locale-independent parsing), or IntlDateFormatter::parse() (for locale-dependent parsing) instead in %s on line %d
strptime(): Argument #1 ($timestamp) must not contain any null bytes

Deprecated: Function strptime() is deprecated since 8.2, use date_parse_from_format() (for locale-independent parsing), or IntlDateFormatter::parse() (for locale-dependent parsing) instead in %s on line %d
strptime(): Argument #2 ($format) must not contain any null bytes
