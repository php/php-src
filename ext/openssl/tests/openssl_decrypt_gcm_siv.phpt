--TEST--
openssl_decrypt() with GCM-SIV cipher algorithm tests
--EXTENSIONS--
openssl
--SKIPIF--
<?php
if (!in_array('aes-256-gcm-siv', openssl_get_cipher_methods())) {
    die("skip aes-256-gcm-siv not available (requires OpenSSL >= 3.2)");
}
?>
--FILE--
<?php
require_once __DIR__ . "/cipher_tests.inc";
$method = 'aes-256-gcm-siv';
$tests = openssl_get_cipher_tests($method);

foreach ($tests as $idx => $test) {
    echo "TEST $idx\n";
    $pt = openssl_decrypt($test['ct'], $method, $test['key'], OPENSSL_RAW_DATA,
        $test['iv'], $test['tag'], $test['aad']);
    var_dump($test['pt'] === $pt);
}

// Use the last vector (non-empty pt + AAD) for tampering checks.
echo "TEST WRONGTAG\n";
$bad_tag = $test['tag'];
$bad_tag[0] = chr(ord($bad_tag[0]) ^ 0x01);
var_dump(openssl_decrypt($test['ct'], $method, $test['key'], OPENSSL_RAW_DATA,
    $test['iv'], $bad_tag, $test['aad']));

echo "TEST WRONGCT\n";
$bad_ct = $test['ct'];
$bad_ct[0] = chr(ord($bad_ct[0]) ^ 0x01);
var_dump(openssl_decrypt($bad_ct, $method, $test['key'], OPENSSL_RAW_DATA,
    $test['iv'], $test['tag'], $test['aad']));

echo "TEST WRONGNONCE\n";
$bad_iv = $test['iv'];
$bad_iv[0] = chr(ord($bad_iv[0]) ^ 0x01);
var_dump(openssl_decrypt($test['ct'], $method, $test['key'], OPENSSL_RAW_DATA,
    $bad_iv, $test['tag'], $test['aad']));

echo "TEST WRONGAAD\n";
var_dump(openssl_decrypt($test['ct'], $method, $test['key'], OPENSSL_RAW_DATA,
    $test['iv'], $test['tag'], 'unexpected aad'));
?>
--EXPECT--
TEST 0
bool(true)
TEST 1
bool(true)
TEST 2
bool(true)
TEST WRONGTAG
bool(false)
TEST WRONGCT
bool(false)
TEST WRONGNONCE
bool(false)
TEST WRONGAAD
bool(false)
