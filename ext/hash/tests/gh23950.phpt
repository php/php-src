--TEST--
MHash: GH-23950 (mhash_keygen_s2k() $length must not be truncated to int)
--SKIPIF--
<?php
if (!function_exists('mhash')) die('skip mhash compatibility layer not available');
if (PHP_INT_SIZE == 4) die('skip only where a PHP integer is wider than an int');
?>
--FILE--
<?php
foreach ([2**31, 2**32, 2**32 + 16, PHP_INT_MAX] as $length) {
    try {
        var_dump(strlen(mhash_keygen_s2k(1, 'password', 'salt', $length)));
    } catch (Throwable $e) {
        echo $e::class, ': ', $e->getMessage(), "\n";
    }
}
?>
--EXPECTF--
Deprecated: Function mhash_keygen_s2k() is deprecated since 8.1 in %s on line %d
ValueError: mhash_keygen_s2k(): Argument #4 ($length) must be less than or equal to 2147483647

Deprecated: Function mhash_keygen_s2k() is deprecated since 8.1 in %s on line %d
ValueError: mhash_keygen_s2k(): Argument #4 ($length) must be less than or equal to 2147483647

Deprecated: Function mhash_keygen_s2k() is deprecated since 8.1 in %s on line %d
ValueError: mhash_keygen_s2k(): Argument #4 ($length) must be less than or equal to 2147483647

Deprecated: Function mhash_keygen_s2k() is deprecated since 8.1 in %s on line %d
ValueError: mhash_keygen_s2k(): Argument #4 ($length) must be less than or equal to 2147483647
