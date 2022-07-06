--TEST--
openssl_decrypt() with GCM cipher algorithm tests
--SKIPIF--
<?php
if (!extension_loaded("openssl"))
    die("skip");
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
    $pt = openssl_decrypt($test['ct'], $method, $test['key'], OPENSSL_RAW_DATA,
        $test['iv'], $test['tag'], $test['aad']);
    var_dump($test['pt'] === $pt);
}

// no IV
var_dump(openssl_decrypt($test['ct'], $method, $test['key'], OPENSSL_RAW_DATA,
    NULL, $test['tag'], $test['aad']));
// failed because no AAD
var_dump(openssl_decrypt($test['ct'], $method, $test['key'], OPENSSL_RAW_DATA,
    $test['iv'], $test['tag']));
// failed because wrong tag
var_dump(openssl_decrypt($test['ct'], $method, $test['key'], OPENSSL_RAW_DATA,
    $test['iv'], str_repeat('x', 16), $test['aad']));

?>
--EXPECTF--
TEST 0
bool(true)
TEST 1
bool(true)
TEST 2
bool(true)
TEST 3
bool(true)
TEST 4
bool(true)
TEST 5
bool(true)

Warning: openssl_decrypt(): Setting of IV length for AEAD mode failed in %s on line %d
bool(false)
bool(false)
bool(false)
