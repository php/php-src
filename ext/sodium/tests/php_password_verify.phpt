--TEST--
Test interoperability of password_verify()
--EXTENSIONS--
sodium
--SKIPIF--
<?php
if (!function_exists('sodium_crypto_pwhash_str')) {
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
];
$memSet = [
  SODIUM_CRYPTO_PWHASH_MEMLIMIT_INTERACTIVE,
  SODIUM_CRYPTO_PWHASH_MEMLIMIT_MODERATE,
];

echo 'Argon2 provider: ';
var_dump(PASSWORD_ARGON2_PROVIDER);

foreach($opsSet as $ops) {
  foreach($memSet as $mem) {
    $password = random_bytes(32);
    echo "Using password: ";
    var_dump(base64_encode($password));
    $hash = sodium_crypto_pwhash_str($password, $ops, $mem);
    echo "Hash: "; var_dump($hash);
    var_dump(password_verify($password, $hash));

    // And verify that incorrect passwords fail.
    $password[0] = chr(ord($password[0]) ^ 1);
    var_dump(password_verify($password, $hash));
  }
}
?>
--EXPECTF--
Argon2 provider: string(%d) "%s"
Using password: string(44) "%s"
Hash: string(97) "$argon2id$v=19$m=65536,t=2,p=1$%s$%s"
bool(true)
bool(false)
Using password: string(44) "%s"
Hash: string(98) "$argon2id$v=19$m=262144,t=2,p=1$%s$%s"
bool(true)
bool(false)
Using password: string(44) "%s"
Hash: string(97) "$argon2id$v=19$m=65536,t=3,p=1$%s$%s"
bool(true)
bool(false)
Using password: string(44) "%s"
Hash: string(98) "$argon2id$v=19$m=262144,t=3,p=1$%s$%s"
bool(true)
bool(false)
