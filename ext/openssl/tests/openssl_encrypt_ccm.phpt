--TEST--
openssl_encrypt() with CCM cipher algorithm tests
--SKIPIF--
<?php
if (!extension_loaded("openssl"))
    die("skip");
if (!in_array('aes-256-ccm', openssl_get_cipher_methods()))
    die("skip: aes-256-ccm not available");
?>
--FILE--
<?php
require_once __DIR__ . "/cipher_tests.inc";
$method = 'aes-256-ccm';
$tests = openssl_get_cipher_tests($method);

foreach ($tests as $idx => $test) {
    echo "TEST $idx\n";
    $ct = openssl_encrypt($test['pt'], $method, $test['key'], OPENSSL_RAW_DATA,
        $test['iv'], $tag, $test['aad'], strlen($test['tag']));
    var_dump($test['ct'] === $ct);
    var_dump($test['tag'] === $tag);
}

// Empty IV error
var_dump(openssl_encrypt('data', $method, 'password', 0, NULL, $tag, ''));

// Test setting different IV length and tag length
var_dump(openssl_encrypt('data', $method, 'password', 0, str_repeat('x', 10), $tag, '', 14));
var_dump(strlen($tag));

// Test setting invalid tag length
var_dump(openssl_encrypt('data', $method, 'password', 0, str_repeat('x', 16), $tag, '', 1024));
?>
--EXPECTF--
TEST 0
bool(true)
bool(true)

Warning: openssl_encrypt(): Setting of IV length for AEAD mode failed in %s on line %d
bool(false)
string(8) "p/lvgA=="
int(14)

Warning: openssl_encrypt(): Setting of IV length for AEAD mode failed in %s on line %d
bool(false)
