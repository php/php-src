--TEST--
sodium_crypto_stream_xchacha20_xor_ic(): key length error names the $key argument
--EXTENSIONS--
sodium
--SKIPIF--
<?php if (!defined('SODIUM_CRYPTO_STREAM_XCHACHA20_KEYBYTES')) print "skip"; ?>
--FILE--
<?php

$nonce = random_bytes(SODIUM_CRYPTO_STREAM_XCHACHA20_NONCEBYTES);

try {
    sodium_crypto_stream_xchacha20_xor_ic('message', $nonce, 0, 'short key');
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}
--EXPECT--
SodiumException: sodium_crypto_stream_xchacha20_xor_ic(): Argument #4 ($key) must be SODIUM_CRYPTO_STREAM_XCHACHA20_KEYBYTES bytes long
