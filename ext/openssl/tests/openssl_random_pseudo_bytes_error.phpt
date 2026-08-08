--TEST--
Test error operation of openssl_random_pseudo_bytes()
--EXTENSIONS--
openssl
--FILE--
<?php
try {
    openssl_random_pseudo_bytes(0);
} catch (Error $e) {
    echo $e::class, ': ', $e->getMessage(), PHP_EOL;
}
?>
--EXPECT--
ValueError: openssl_random_pseudo_bytes(): Argument #1 ($length) must be greater than 0
