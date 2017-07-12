--TEST--
Check for libsodium ed25519 signatures
--SKIPIF--
<?php if (!extension_loaded("sodium")) print "skip"; ?>
--FILE--
<?php
$keypair = sodium_crypto_sign_keypair();
var_dump(strlen($keypair) === SODIUM_CRYPTO_SIGN_KEYPAIRBYTES);
$sk = sodium_crypto_sign_secretkey($keypair);
var_dump(strlen($sk) === SODIUM_CRYPTO_SIGN_SECRETKEYBYTES);
$pk = sodium_crypto_sign_publickey($keypair);
var_dump(strlen($pk) === SODIUM_CRYPTO_SIGN_PUBLICKEYBYTES);
var_dump($pk !== $sk);
$keypair2 = sodium_crypto_sign_keypair_from_secretkey_and_publickey($sk, $pk);
var_dump($keypair === $keypair2);

$alice_kp = sodium_crypto_sign_keypair();
$alice_secretkey = sodium_crypto_sign_secretkey($alice_kp);
$alice_publickey = sodium_crypto_sign_publickey($alice_kp);

$msg = "Here is the message, to be signed using Alice's secret key, and " .
  "to be verified using Alice's public key";

$msg_signed = sodium_crypto_sign($msg, $alice_secretkey);
var_dump(strlen($msg_signed) - strlen($msg) === SODIUM_CRYPTO_SIGN_BYTES);

$msg_orig = sodium_crypto_sign_open($msg_signed, $alice_publickey);
var_dump($msg_orig === $msg);

$seed = str_repeat('x', SODIUM_CRYPTO_SIGN_SEEDBYTES);
$alice_kp = sodium_crypto_sign_seed_keypair($seed);

$alice_secretkey = sodium_crypto_sign_secretkey($alice_kp);
$alice_publickey = sodium_crypto_sign_publickey($alice_kp);

$msg = "Here is another message, to be signed using Alice's secret key, and " .
  "to be verified using Alice's public key, which will be always the same " .
  "since they are derived from a fixed seed";

$msg_signed = sodium_crypto_sign($msg, $alice_secretkey);
var_dump(strlen($msg_signed) - strlen($msg) === SODIUM_CRYPTO_SIGN_BYTES);

$msg_orig = sodium_crypto_sign_open($msg_signed, $alice_publickey);
var_dump($msg_orig === $msg);

$signature = sodium_crypto_sign_detached($msg, $alice_secretkey);
var_dump(strlen($signature) === SODIUM_CRYPTO_SIGN_BYTES);
var_dump(sodium_crypto_sign_verify_detached($signature,
											 $msg, $alice_publickey));
var_dump(sodium_crypto_sign_verify_detached($signature,
											 $msg . "\0", $alice_publickey));

$calc_pubkey = sodium_crypto_sign_publickey_from_secretkey($alice_secretkey);
var_dump(sodium_memcmp($calc_pubkey, $alice_publickey) === 0);

$ed25519key = sodium_hex2bin("55b62f664bf1c359f58a6b91b89556f97284273510573055b9237d17f5a20564607f0626f49e63c2c8f814ed6d955bf8b005b33fd5fd56eaca93073d8eb99165");
$curve25519key = sodium_crypto_sign_ed25519_sk_to_curve25519($ed25519key);
var_dump($curve25519key === sodium_hex2bin("381b2be5e3d38820deb1243fb58b4be654da30dd3ccde492cb88f937eb489363"));

try {
	sodium_crypto_sign($msg, substr($alice_secretkey, 1));
} catch (SodiumException $ex) {
	var_dump(true);
}
?>
--EXPECT--
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(false)
bool(true)
bool(true)
bool(true)
