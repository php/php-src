--TEST--
openssl_encrypt() with GCM cipher algorithm tests
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
	$ct = openssl_encrypt($test['pt'], $method, $test['key'], OPENSSL_RAW_DATA,
		$test['iv'], $tag, $test['aad'], strlen($test['tag']));
	var_dump($test['ct'] === $ct);
	var_dump($test['tag'] === $tag);
}

?>
--EXPECT--
TEST 0
bool(true)
bool(true)
