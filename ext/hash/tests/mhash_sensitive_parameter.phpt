--TEST--
Test that the key/password parameters of mhash() and mhash_keygen_s2k() are marked sensitive.
--SKIPIF--
<?php if (!function_exists('mhash')) { die('skip mhash compatibility layer not available'); } ?>
--FILE--
<?php
declare(strict_types=1);

try {
    var_dump(mhash(0, null, 'secret-key'));
} catch (\Throwable $e) {
    echo $e, PHP_EOL;
}
try {
    var_dump(mhash_keygen_s2k(0, 'secret-password', 'salt', 0));
} catch (\Throwable $e) {
    echo $e, PHP_EOL;
}
?>
--EXPECTF--
Deprecated: Function mhash() is deprecated since 8.1 in %s on line %d
TypeError: mhash(): Argument #2 ($data) must be of type string, null given in %s:%d
Stack trace:
#0 %s(%d): mhash(0, NULL, Object(SensitiveParameterValue))
#1 {main}

Deprecated: Function mhash_keygen_s2k() is deprecated since 8.1 in %s on line %d
ValueError: mhash_keygen_s2k(): Argument #4 ($length) must be a greater than 0 in %s:%d
Stack trace:
#0 %s(%d): mhash_keygen_s2k(0, Object(SensitiveParameterValue), 'salt', 0)
#1 {main}
