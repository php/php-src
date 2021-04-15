--TEST--
Check for libsodium stream
--SKIPIF--
<?php if (!extension_loaded("sodium") || !defined('CRYPTO_STREAM_XCHACHA20_KEYBYTES')) print "skip"; ?>
--FILE--
<?php
$nonce = random_bytes(SODIUM_CRYPTO_STREAM_XCHACHA20_NONCEBYTES);
$key = sodium_crypto_stream_xchacha20_keygen();

$len = 100;
$stream = sodium_crypto_stream_xchacha20($len, $nonce, $key);
var_dump(strlen($stream));

$stream2 = sodium_crypto_stream_xchacha20($len, $nonce, $key);

$nonce = random_bytes(SODIUM_CRYPTO_STREAM_XCHACHA20_NONCEBYTES);
$stream3 = sodium_crypto_stream_xchacha20($len, $nonce, $key);

$key = sodium_crypto_stream_keygen();
$stream4 = sodium_crypto_stream_xchacha20($len, $nonce, $key);

var_dump($stream === $stream2);
var_dump($stream !== $stream3);
var_dump($stream !== $stream4);
var_dump($stream2 !== $stream3);
var_dump($stream2 !== $stream4);
var_dump($stream3 !== $stream4);

$stream5 = sodium_crypto_stream_xchacha20_xor($stream, $nonce, $key);
var_dump($stream5 !== $stream);
$stream6 = sodium_crypto_stream_xchacha20_xor($stream5, $nonce, $key);

var_dump($stream6 === $stream);

try {
    sodium_crypto_stream_xchacha20($len, substr($nonce, 1), $key);
} catch (SodiumException $ex) {
    var_dump(true);
}

?>
--EXPECT--
int(100)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
