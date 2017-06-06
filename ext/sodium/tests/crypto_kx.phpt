--TEST--
Check for libsodium-based key exchange
--SKIPIF--
<?php if (!extension_loaded("sodium")) print "skip"; ?>
--FILE--
<?php
$client_secretkey = sodium_hex2bin("8520f0098930a754748b7ddcb43ef75a0dbf3a0d26381af4eba4a98eaa9b4e6a");
$client_publickey = sodium_crypto_box_publickey_from_secretkey($client_secretkey);

$server_secretkey = sodium_hex2bin("948f00e90a246fb5909f8648c2ac6f21515771235523266439e0d775ba0c3671");
$server_publickey = sodium_crypto_box_publickey_from_secretkey($server_secretkey);

$shared_key_computed_by_client =
  sodium_crypto_kx($client_secretkey, $server_publickey,
					$client_publickey, $server_publickey);

$shared_key_computed_by_server =
  sodium_crypto_kx($server_secretkey, $client_publickey,
					$client_publickey, $server_publickey);

var_dump(sodium_bin2hex($shared_key_computed_by_client));
var_dump(sodium_bin2hex($shared_key_computed_by_server));
try {
	sodium_crypto_kx(
		substr($client_secretkey, 1),
		$server_publickey,
		$client_publickey,
		$server_publickey
	);
} catch (SodiumException $ex) {
	var_dump(true);
}
?>
--EXPECT--
string(64) "509a1580c2ee30c565317e29e0fea0b1c232e0ef3a7871d91dc64814b19a3bd2"
string(64) "509a1580c2ee30c565317e29e0fea0b1c232e0ef3a7871d91dc64814b19a3bd2"
bool(true)
