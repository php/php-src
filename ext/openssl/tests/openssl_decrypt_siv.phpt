--TEST--
openssl_decrypt() with SIV cipher algorithm tests
--EXTENSIONS--
openssl
--SKIPIF--
<?php
if (!in_array('aes-128-siv', openssl_get_cipher_methods()))
    die("skip: aes-128-siv not available");
?>
--FILE--
<?php
require_once __DIR__ . "/cipher_tests.inc";
$method = 'aes-128-siv';
$tests = openssl_get_cipher_tests($method);

foreach ($tests as $idx => $test) {
    echo "TEST $idx\n";
    $pt = openssl_decrypt($test['ct'], $method, $test['key'], OPENSSL_RAW_DATA,
        $test['iv'], $test['tag'], $test['aad']);
    var_dump($test['pt'] === $pt);
}

// failed because no AAD
echo "TEST AAD\n";
var_dump(openssl_decrypt($test['ct'], $method, $test['key'], OPENSSL_RAW_DATA,
    $test['iv'], $test['tag']));
// failed because wrong tag
echo "TEST WRONGTAG\n";
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
TEST AAD
bool(false)
TEST WRONGTAG
bool(false)
