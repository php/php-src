--TEST--
Check for libsodium KDF
--EXTENSIONS--
sodium
--FILE--
<?php
$key = sodium_crypto_kdf_keygen();
try {
  $subkey = sodium_crypto_kdf_derive_from_key(10, 0, "context!", $key);
} catch (SodiumException $ex) {
  var_dump(true);
}
try {
  $subkey = sodium_crypto_kdf_derive_from_key(100, 0, "context!", $key);
} catch (SodiumException $ex) {
  var_dump(true);
}
try {
  $subkey = sodium_crypto_kdf_derive_from_key(SODIUM_CRYPTO_KDF_BYTES_MAX, 0, "context", $key);
} catch (SodiumException $ex) {
  var_dump(true);
}
try {
  $subkey = sodium_crypto_kdf_derive_from_key(SODIUM_CRYPTO_KDF_BYTES_MAX, -1, "context!", $key);
} catch (SodiumException $ex) {
  var_dump(true);
}
try {
  $subkey = sodium_crypto_kdf_derive_from_key(SODIUM_CRYPTO_KDF_BYTES_MAX, 0, "context!", "short key");
} catch (SodiumException $ex) {
  var_dump(true);
}

$subkey1 = sodium_crypto_kdf_derive_from_key(SODIUM_CRYPTO_KDF_BYTES_MIN, 0, "context!", $key);
$subkey2 = sodium_crypto_kdf_derive_from_key(SODIUM_CRYPTO_KDF_BYTES_MIN, 1, "context!", $key);
$subkey3 = sodium_crypto_kdf_derive_from_key(SODIUM_CRYPTO_KDF_BYTES_MIN, 2, "context2", $key);
$subkey4 = sodium_crypto_kdf_derive_from_key(SODIUM_CRYPTO_KDF_BYTES_MIN, 0, "context!", $key);

var_dump($subkey1 !== $subkey2);
var_dump($subkey1 !== $subkey3);
var_dump($subkey2 !== $subkey3);
var_dump($subkey1 === $subkey4);

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
