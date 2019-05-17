--TEST--
Check for libsodium secretstream
--SKIPIF--
<?php
if (!extension_loaded("sodium")) print "skip extension not loaded";
if (!defined('SODIUM_CRYPTO_SECRETSTREAM_XCHACHA20POLY1305_ABYTES')) print "skip libsodium without secretbytes";
?>
--FILE--
<?php
echo "secretstream_xchacha20poly1305:\n";

$msg1 = random_bytes(random_int(1, 1000));
$msg2 = random_bytes(random_int(1, 1000));
$key = sodium_crypto_secretstream_xchacha20poly1305_keygen();
$ad = random_bytes(random_int(1, 1000));

$res = sodium_crypto_secretstream_xchacha20poly1305_init_push($key);
$stream = $res[0];
$header = $res[1];

$c1 = sodium_crypto_secretstream_xchacha20poly1305_push($stream, $msg1, $ad);
$c2 = sodium_crypto_secretstream_xchacha20poly1305_push($stream, $msg2, '', SODIUM_CRYPTO_SECRETSTREAM_XCHACHA20POLY1305_TAG_FINAL);

$stream = sodium_crypto_secretstream_xchacha20poly1305_init_pull($header, $key);

$r1 = sodium_crypto_secretstream_xchacha20poly1305_pull($stream, $c1, $ad);
$r2 = sodium_crypto_secretstream_xchacha20poly1305_pull($stream, $c2);

var_dump($msg1 === $r1[0]);
var_dump($msg2 === $r2[0]);
var_dump($r1[1]);
var_dump($r2[1]);

var_dump(sodium_crypto_secretstream_xchacha20poly1305_pull($stream, $c2));
var_dump(sodium_crypto_secretstream_xchacha20poly1305_pull($stream, 'x'));

$stream = sodium_crypto_secretstream_xchacha20poly1305_init_pull($header, $key);
$r1 = sodium_crypto_secretstream_xchacha20poly1305_pull($stream, $c1);
var_dump($r1);
$r1 = sodium_crypto_secretstream_xchacha20poly1305_pull($stream, $c1, $ad);
var_dump($msg1 === $r1[0]);

$stream = sodium_crypto_secretstream_xchacha20poly1305_init_pull($header, $key);
sodium_crypto_secretstream_xchacha20poly1305_rekey($stream);
$r1 = sodium_crypto_secretstream_xchacha20poly1305_pull($stream, $c1, $ad);
var_dump($r1);
$r2 = sodium_crypto_secretstream_xchacha20poly1305_pull($stream, $c2);
var_dump($r2);
--EXPECT--
secretstream_xchacha20poly1305:
bool(true)
bool(true)
int(0)
int(3)
bool(false)
bool(false)
bool(false)
bool(true)
bool(false)
bool(false)
