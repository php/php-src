--TEST--
openssl_decrypt() with CCM cipher algorithm tests
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
$methods = ['aes-128-ccm', 'aes-256-ccm'];

foreach ($methods as $method) {
    $tests = openssl_get_cipher_tests($method);
    foreach ($tests as $idx => $test) {
        echo "$method - TEST $idx\n";
        $pt = openssl_decrypt($test['ct'], $method, $test['key'], OPENSSL_RAW_DATA,
            $test['iv'], $test['tag'], $test['aad']);
        var_dump($test['pt'] === $pt);
    }
}

// no IV
var_dump(openssl_decrypt($test['ct'], $method, $test['key'], OPENSSL_RAW_DATA,
    NULL, $test['tag'], $test['aad']));
// failed because no AAD
var_dump(openssl_decrypt($test['ct'], $method, $test['key'], OPENSSL_RAW_DATA,
    $test['iv'], $test['tag']));
// failed because wrong tag
var_dump(openssl_decrypt($test['ct'], $method, $test['key'], OPENSSL_RAW_DATA,
    $test['iv'], str_repeat('x', 10), $test['aad']));

?>
--EXPECTF--
aes-128-ccm - TEST 0
bool(true)
aes-128-ccm - TEST 1
bool(true)
aes-256-ccm - TEST 0
bool(true)

Warning: openssl_decrypt(): Setting of IV length for AEAD mode failed in %s on line %d
bool(false)
bool(false)
bool(false)
