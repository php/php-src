--TEST--
openssl_encrypt() with ChaCha20 and Poly1305 cipher algorithm tests
--EXTENSIONS--
openssl
--SKIPIF--
<?php
if (!in_array('chacha20-poly1305', openssl_get_cipher_methods()))
    die("skip: chacha20-poly1305 not available");
?>
--FILE--
<?php
require_once __DIR__ . "/cipher_tests.inc";
$method = 'chacha20-poly1305';
$tests = openssl_get_cipher_tests($method);

foreach ($tests as $idx => $test) {
    echo "TEST $idx\n";
    $ct = openssl_encrypt($test['pt'], $method, $test['key'], OPENSSL_RAW_DATA,
        $test['iv'], $tag, $test['aad'], strlen($test['tag']));
    var_dump($test['ct'] === $ct);
    var_dump($test['tag'] === $tag);
}

// Empty IV error
var_dump(openssl_encrypt('data', $method, 'password', 0, '', $tag, ''));

// Failing to retrieve tag (max is 16 bytes)
var_dump(openssl_encrypt('data', $method, 'password', 0, str_repeat('x', 12), $tag, '', 20));

// Failing when no tag supplied
var_dump(openssl_encrypt('data', $method, 'password', 0, str_repeat('x', 12)));
?>
--EXPECTF--
TEST 0
bool(true)
bool(true)
TEST 1
bool(true)
bool(true)

Warning: openssl_encrypt(): Setting of IV length for AEAD mode failed in %s on line %d
bool(false)

Warning: openssl_encrypt(): Retrieving verification tag failed in %s on line %d
bool(false)

Warning: openssl_encrypt(): A tag should be provided when using AEAD mode in %s on line %d
bool(false)
