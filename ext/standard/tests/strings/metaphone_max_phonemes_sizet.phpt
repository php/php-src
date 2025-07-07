--TEST--
metaphone() $maxPhonemes overflow on narrow size_t
--SKIPIF--
<?php
if (PHP_SYS_SIZE >= PHP_INT_SIZE) {
    die("skip size_t is not narrower than zend_long on this platform");
}
?>
--FILE--
<?php
try {
    metaphone('hello', PHP_INT_MAX);
    echo "unexpected success\n";
} catch (ValueError $e) {
    echo $e->getMessage(), "\n";
}
?>
--EXPECTF--
Deprecated: Function metaphone() is deprecated since 8.6, use a userland phonetic matching library instead in %s on line %d
metaphone(): Argument #2 ($max_phonemes) must be less than or equal to %d
