--TEST--
Test error operation of openssl_random_pseudo_bytes()
--SKIPIF--
<?php if (!extension_loaded("openssl")) print "skip"; ?>
--FILE--
<?php
try {
    openssl_random_pseudo_bytes(0);
} catch (Error $e) {
    echo $e->getMessage().PHP_EOL;
}
?>
--EXPECT--
Length must be greater than 0
