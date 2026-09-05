--TEST--
Test interoperability of password_hash('argon2i')
--EXTENSIONS--
sodium
--SKIPIF--
<?php
if (!function_exists('sodium_crypto_pwhash_str_verify')) {
  echo "skip - No crypto_pwhash_str_verify";
}
if (!in_array('argon2i', password_algos(), true /* strict */)) {
  echo "skip - No argon2i support in password_hash()";
}
?>
--FILE--
<?php

echo 'Argon2 provider: ';
var_dump(PASSWORD_ARGON2_PROVIDER);

// Interoperability does not depend on using production-strength costs.
foreach([1, 2] as $mem) {
  foreach([3, 4] as $time) {
    $opts = [
      'memory_cost' => 8 * 1024 * $mem,
      'time_cost'   => $time,
      'threads'     => PASSWORD_ARGON2_DEFAULT_THREADS,
    ];
    $password = random_bytes(32);
    echo "Using password: ";
    var_dump(base64_encode($password));
    $hash = password_hash($password, 'argon2i', $opts);
    echo "Hash: "; var_dump($hash);
    var_dump(sodium_crypto_pwhash_str_verify($hash, $password));

    // And verify that incorrect passwords fail.
    $password[0] = chr(ord($password[0]) ^ 1);
    var_dump(sodium_crypto_pwhash_str_verify($hash, $password));
  }
}
?>
--EXPECTF--
Argon2 provider: string(%d) "%s"
Using password: string(44) "%s"
Hash: string(95) "$argon2i$v=19$m=8192,t=3,p=1$%s$%s"
bool(true)
bool(false)
Using password: string(44) "%s"
Hash: string(95) "$argon2i$v=19$m=8192,t=4,p=1$%s$%s"
bool(true)
bool(false)
Using password: string(44) "%s"
Hash: string(96) "$argon2i$v=19$m=16384,t=3,p=1$%s$%s"
bool(true)
bool(false)
Using password: string(44) "%s"
Hash: string(96) "$argon2i$v=19$m=16384,t=4,p=1$%s$%s"
bool(true)
bool(false)
