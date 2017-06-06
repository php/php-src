--TEST--
Check for libsodium utils
--SKIPIF--
<?php if (!extension_loaded("sodium")) print "skip"; ?>
--FILE--
<?php
$passwd = 'test';

$hash = sodium_crypto_pwhash_scryptsalsa208sha256_str
  ($passwd, SODIUM_CRYPTO_PWHASH_SCRYPTSALSA208SHA256_OPSLIMIT_INTERACTIVE,
			SODIUM_CRYPTO_PWHASH_SCRYPTSALSA208SHA256_MEMLIMIT_INTERACTIVE);
var_dump(substr($hash, 0, 3) ===
		 SODIUM_CRYPTO_PWHASH_SCRYPTSALSA208SHA256_STRPREFIX);

$c = sodium_crypto_pwhash_scryptsalsa208sha256_str_verify($hash, $passwd);
var_dump($c);

$c = sodium_crypto_pwhash_scryptsalsa208sha256_str_verify($hash, 'passwd');
var_dump($c);

$salt = random_bytes(SODIUM_CRYPTO_PWHASH_SCRYPTSALSA208SHA256_SALTBYTES);
$out_len = 100;
$key = sodium_crypto_pwhash_scryptsalsa208sha256
  ($out_len, $passwd, $salt,
   SODIUM_CRYPTO_PWHASH_SCRYPTSALSA208SHA256_OPSLIMIT_INTERACTIVE,
   SODIUM_CRYPTO_PWHASH_SCRYPTSALSA208SHA256_MEMLIMIT_INTERACTIVE);
var_dump(strlen($key) === $out_len);
?>
--EXPECT--
bool(true)
bool(true)
bool(false)
bool(true)
