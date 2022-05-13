--TEST--
openssl_encrypt() with CCM cipher algorithm tests
--EXTENSIONS--
openssl
--SKIPIF--
<?php
if (!in_array('aes-256-ccm', openssl_get_cipher_methods()))
    die("skip: aes-256-ccm not available");
?>
--FILE--
<?php
require_once __DIR__ . "/cipher_tests.inc";
$methods = ['aes-128-ccm', 'aes-256-ccm'];

foreach ($methods as $method) {
    $tests = openssl_get_cipher_tests($method);
    foreach ($tests as $idx => $test) {
        echo "$method - TEST $idx\n";
        $ct = openssl_encrypt($test['pt'], $method, $test['key'], OPENSSL_RAW_DATA,
            $test['iv'], $tag, $test['aad'], strlen($test['tag']));
        var_dump($test['ct'] === $ct);
        var_dump($test['tag'] === $tag);
    }
}

// Empty IV error
var_dump(openssl_encrypt('data', $method, 'password', 0, '', $tag, ''));

// Test setting different IV length and tag length
var_dump(openssl_encrypt('data', $method, 'password', 0, str_repeat('x', 10), $tag, '', 14));
var_dump(strlen($tag));

// Test setting invalid tag length
var_dump(openssl_encrypt('data', $method, 'password', 0, str_repeat('x', 16), $tag, '', 1024));
?>
--EXPECTF--
aes-128-ccm - TEST 0
bool(true)
bool(true)
aes-128-ccm - TEST 1
bool(true)
bool(true)
aes-256-ccm - TEST 0
bool(true)
bool(true)

Warning: openssl_encrypt(): Setting of IV length for AEAD mode failed in %s on line %d
bool(false)
string(8) "p/lvgA=="
int(14)

Warning: openssl_encrypt(): Setting of IV length for AEAD mode failed in %s on line %d
bool(false)
