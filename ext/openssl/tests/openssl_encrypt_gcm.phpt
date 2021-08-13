--TEST--
openssl_encrypt() with GCM cipher algorithm tests
--EXTENSIONS--
openssl
--SKIPIF--
<?php
if (!in_array('aes-128-gcm', openssl_get_cipher_methods()))
    die("skip: aes-128-gcm not available");
?>
--FILE--
<?php
require_once __DIR__ . "/cipher_tests.inc";
$method = 'aes-128-gcm';
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
var_dump(openssl_encrypt('data', $method, 'password', 0, str_repeat('x', 32), $tag, '', 20));

// Failing when no tag supplied
var_dump(openssl_encrypt('data', $method, 'password', 0, str_repeat('x', 32)));
?>
--EXPECTF--
TEST 0
bool(true)
bool(true)
TEST 1
bool(true)
bool(true)
TEST 2
bool(true)
bool(true)
TEST 3
bool(true)
bool(true)
TEST 4
bool(true)
bool(true)
TEST 5
bool(true)
bool(true)

Warning: openssl_encrypt(): Setting of IV length for AEAD mode failed in %s on line %d
bool(false)

Warning: openssl_encrypt(): Retrieving verification tag failed in %s on line %d
bool(false)

Warning: openssl_encrypt(): A tag should be provided when using AEAD mode in %s on line %d
bool(false)
