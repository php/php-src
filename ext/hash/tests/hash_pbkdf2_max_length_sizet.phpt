--TEST--
Hash: hash_pbkdf2() function : output length beyond ZSTR_MAX_LEN where size_t is narrower than zend_long
--SKIPIF--
<?php
if (PHP_SYS_SIZE >= PHP_INT_SIZE) {
    die("skip size_t is not narrower than zend_long on this platform");
}
?>
--FILE--
<?php
/* The number of digest blocks derived from $length was narrowed to size_t
 * when sizing the result buffer. For PHP_INT_MAX it wrapped to 0, so the
 * buffer was allocated empty and every block was written past its end. */
foreach ([false, true] as $raw) {
    try {
        hash_pbkdf2('md5', 'password', 'salt', 1, PHP_INT_MAX, $raw);
    } catch (ValueError $e) {
        echo $e->getMessage(), "\n";
    }
}
?>
--EXPECTF--
hash_pbkdf2(): Argument #5 ($length) must be less than or equal to %d
hash_pbkdf2(): Argument #5 ($length) must be less than or equal to %d
