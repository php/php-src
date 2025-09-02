--TEST--
Check for libsodium stream
--EXTENSIONS--
sodium
--SKIPIF--
<?php if (!defined('SODIUM_CRYPTO_STREAM_XCHACHA20_KEYBYTES')) print "skip"; ?>
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

// New test (with Initial Counter feature):
$n2 = random_bytes(SODIUM_CRYPTO_STREAM_XCHACHA20_NONCEBYTES);
$left  = str_repeat("\x01", 64);
$right = str_repeat("\xfe", 64);

// All at once:
$stream7_unified = sodium_crypto_stream_xchacha20_xor($left . $right, $n2, $key);

// Piecewise, with initial counter:
$stream7_left  = sodium_crypto_stream_xchacha20_xor_ic($left, $n2, 0, $key);
$stream7_right = sodium_crypto_stream_xchacha20_xor_ic($right, $n2, 1, $key);
$stream7_concat = $stream7_left . $stream7_right;

var_dump(strlen($stream7_concat));
var_dump($stream7_unified === $stream7_concat);

try {
    sodium_crypto_stream_xchacha20(-1, $nonce, $key);
} catch (SodiumException $ex) {
    echo $ex->getMessage(), "\n";
}
try {
    sodium_crypto_stream_xchacha20($len, substr($nonce, 1), $key);
} catch (SodiumException $ex) {
    echo $ex->getMessage(), "\n";
}
try {
    sodium_crypto_stream_xchacha20($len, $nonce, substr($key, 1));
} catch (SodiumException $ex) {
    echo $ex->getMessage(), "\n";
}
try {
    sodium_crypto_stream_xchacha20_xor($stream, substr($nonce, 1), $key);
} catch (SodiumException $ex) {
    echo $ex->getMessage(), "\n";
}
try {
    sodium_crypto_stream_xchacha20_xor($stream, $nonce, substr($key, 1));
} catch (SodiumException $ex) {
    echo $ex->getMessage(), "\n";
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
int(128)
bool(true)
sodium_crypto_stream_xchacha20(): Argument #1 ($length) must be greater than 0
sodium_crypto_stream_xchacha20(): Argument #2 ($nonce) must be SODIUM_CRYPTO_STREAM_XCHACHA20_NONCEBYTES bytes long
sodium_crypto_stream_xchacha20(): Argument #3 ($key) must be SODIUM_CRYPTO_STREAM_XCHACHA20_KEYBYTES bytes long
sodium_crypto_stream_xchacha20_xor(): Argument #2 ($nonce) must be SODIUM_CRYPTO_STREAM_XCHACHA20_NONCEBYTES bytes long
sodium_crypto_stream_xchacha20_xor(): Argument #3 ($key) must be SODIUM_CRYPTO_STREAM_XCHACHA20_KEYBYTES bytes long
