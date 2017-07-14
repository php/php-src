--TEST--
Check for libsodium-based key exchange
--SKIPIF--
<?php if (!extension_loaded("sodium")) print "skip"; ?>
--FILE--
<?php
$client_seed = sodium_hex2bin('0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef');
$client_keypair = sodium_crypto_kx_seed_keypair($client_seed);
$server_seed = sodium_hex2bin('f123456789abcdef0123456789abcdef0123456789abcdef0123456789abcde0');
$server_keypair = sodium_crypto_kx_seed_keypair($server_seed);

var_dump(sodium_bin2hex($client_keypair));
var_dump(sodium_bin2hex($server_keypair));

$client_session_keys =
  sodium_crypto_kx_client_session_keys($client_keypair,
    sodium_crypto_kx_publickey($server_keypair));

$server_session_keys =
  sodium_crypto_kx_server_session_keys($server_keypair,
    sodium_crypto_kx_publickey($client_keypair));

var_dump(sodium_bin2hex($client_session_keys[0]));
var_dump(sodium_bin2hex($server_session_keys[1]));
var_dump(sodium_bin2hex($client_session_keys[1]));
var_dump(sodium_bin2hex($server_session_keys[0]));
?>
--EXPECT--
string(128) "b85c84f9828524519d32b97cd3dda961fdba2dbf407ae4601e2129229aa463c224eaf70f070a925d6d5176f20495d4d90867624d9a10379e2a9aef0955c9bf4e"
string(128) "016e814c32b8b66225a403db45bf50fdd1966fb802c3115bf8aa90738c6a02de420ccdb534930fed9aaff12188bedc76e66251f399c404f2e4a15678fd4a484a"
string(64) "99a430e61d718b71979ebcea6735c4648bc828cfb456890aeda4b628b77d5ac7"
string(64) "99a430e61d718b71979ebcea6735c4648bc828cfb456890aeda4b628b77d5ac7"
string(64) "876bef865a5ab3f4ae569ea5aaefe5014c3ec22a558c0a2f0274aa9985bd328d"
string(64) "876bef865a5ab3f4ae569ea5aaefe5014c3ec22a558c0a2f0274aa9985bd328d"
