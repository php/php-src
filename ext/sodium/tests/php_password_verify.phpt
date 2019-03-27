--TEST--
Test interoperability of password_verify()
--SKIPIF--
<?php
if (!function_exits('sodium_crypto_pwhash_str')) {
  echo "skip - No crypto_pwhash_str_verify";
}

// Depending on library version, libsodium may provide argon2i or argon2id hashes.
$hash = sodium_crypto_pwhash_str("test", SODIUM_CRYPTO_PWHASH_OPSLIMIT_INTERACTIVE, SODIUM_CRYPTO_PWHASH_MEMLIMIT_INTERACTIVE);
list(, $algo) = explode('$', $hash, 3);

if (!in_array($algo, password_algos(), true /* strict */)) {
 echo "skip - No $algo support in password_verify()";
}
--FILE--
<?php

$opsSet = [
  SODIUM_CRYPTO_PWHASH_OPSLIMIT_INTERACTIVE,
  SODIUM_CRYPTO_PWHASH_OPSLIMIT_MODERATE,
  SODIUM_CRYPTO_PWHASH_OPSLIMIT_SENSITIVE,
];
$memSet = [
  SODIUM_CRYPTO_PWHASH_MEMLIMIT_INTERACTIVE,
  SODIUM_CRYPTO_PWHASH_MEMLIMIT_MODERATE,
  SODIUM_CRYPTO_PWHASH_MEMLIMIT_SENSITIVE,
];

foreach($opsSet as $ops) {
  foreach($memSet as $mem) {
    $password = random_bytes(32);
    echo "Using password: ";
    var_dump(base64_encode($password));
    $hash = sodium_crypto_pwhash_str($password, $ops, $mem);
    echo "Hash: "; var_dump($hash);
    var_dump(password_verify($password, $hash));
  }
}
--EXPECTF--
Using password: string(44) "%s"
Hash: string(%d) "$argon2i%s"
bool(true)
Using password: string(44) "%s"
Hash: string(%d) "$argon2i%s"
bool(true)
Using password: string(44) "%s"
Hash: string(%d) "$argon2i%s"
bool(true)
Using password: string(44) "%s"
Hash: string(%d) "$argon2i%s"
bool(true)
Using password: string(44) "%s"
Hash: string(%d) "$argon2i%s"
bool(true)
Using password: string(44) "%s"
Hash: string(%d) "$argon2i%s"
bool(true)
Using password: string(44) "%s"
Hash: string(%d) "$argon2i%s"
bool(true)
Using password: string(44) "%s"
Hash: string(%d) "$argon2i%s"
bool(true)
Using password: string(44) "%s"
Hash: string(%d) "$argon2i%s"
bool(true)
