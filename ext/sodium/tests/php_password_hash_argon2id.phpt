--TEST--
Test interoperability of password_hash('argon2id')
--SKIPIF--
<?php
if (!function_exits('sodium_crypto_pwhash_str_verify')) {
  echo "skip - No crypto_pwhash_str_verify";
}
if (!in_array('argon2id', password_algos(), true /* strict */)) {
  echo "skip - No argon2id support in password_hash()";
}
--FILE--
<?php

foreach([1, 2, 4] as $mem) {
  foreach([1, 2, 4] as $time) {
    foreach([1, 2, 4] as $threads) {
      $opts = [
        'memory_cost' => PASSWORD_ARGON2_DEFAULT_MEMORY_COST * $mem,
        'time_cost'   => PASSWORD_ARGON2_DEFAULT_TIME_COST * $time,
        'threads'     => PASSWORD_ARGON2_DEFAULT_THREADS * $threads,
      ];
      $password = random_bytes(32);
      echo "Using password: ";
      var_dump(base64_encode($password));
      $hash = password_hash($password, 'argon2id', $opts);
      echo "Hash: "; var_dump($hash);
      var_dump(sodium_crypto_pwhash_str_verify($hash, $password));
    }
  }
}
--EXPECTF--
Using password: string(44) "%s"
Hash: string(96) "$argon2id$v=19$m=1024,t=3,p=2$%s"
bool(true)
Using password: string(44) "%s"
Hash: string(96) "$argon2id$v=19$m=1024,t=3,p=4$%s"
bool(true)
Using password: string(44) "%s"
Hash: string(96) "$argon2id$v=19$m=1024,t=3,p=8$%s"
bool(true)
Using password: string(44) "%s"
Hash: string(96) "$argon2id$v=19$m=1024,t=6,p=2$%s"
bool(true)
Using password: string(44) "%s"
Hash: string(96) "$argon2id$v=19$m=1024,t=6,p=4$%s"
bool(true)
Using password: string(44) "%s"
Hash: string(96) "$argon2id$v=19$m=1024,t=6,p=8$%s"
bool(true)
Using password: string(44) "%s"
Hash: string(97) "$argon2id$v=19$m=1024,t=12,p=2$%s"
bool(true)
Using password: string(44) "%s"
Hash: string(97) "$argon2id$v=19$m=1024,t=12,p=4$%s"
bool(true)
Using password: string(44) "%s"
Hash: string(97) "$argon2id$v=19$m=1024,t=12,p=8$%s"
bool(true)
Using password: string(44) "%s"
Hash: string(96) "$argon2id$v=19$m=2048,t=3,p=2$%s"
bool(true)
Using password: string(44) "%s"
Hash: string(96) "$argon2id$v=19$m=2048,t=3,p=4$%s"
bool(true)
Using password: string(44) "%s"
Hash: string(96) "$argon2id$v=19$m=2048,t=3,p=8$%s"
bool(true)
Using password: string(44) "%s"
Hash: string(96) "$argon2id$v=19$m=2048,t=6,p=2$%s"
bool(true)
Using password: string(44) "%s"
Hash: string(96) "$argon2id$v=19$m=2048,t=6,p=4$%s"
bool(true)
Using password: string(44) "%s"
Hash: string(96) "$argon2id$v=19$m=2048,t=6,p=8$%s"
bool(true)
Using password: string(44) "%s"
Hash: string(97) "$argon2id$v=19$m=2048,t=12,p=2$%s"
bool(true)
Using password: string(44) "%s"
Hash: string(97) "$argon2id$v=19$m=2048,t=12,p=4$%s"
bool(true)
Using password: string(44) "%s"
Hash: string(97) "$argon2id$v=19$m=2048,t=12,p=8$%s"
bool(true)
Using password: string(44) "%s"
Hash: string(96) "$argon2id$v=19$m=4096,t=3,p=2$%s"
bool(true)
Using password: string(44) "%s"
Hash: string(96) "$argon2id$v=19$m=4096,t=3,p=4$%s"
bool(true)
Using password: string(44) "%s"
Hash: string(96) "$argon2id$v=19$m=4096,t=3,p=8$%s"
bool(true)
Using password: string(44) "%s"
Hash: string(96) "$argon2id$v=19$m=4096,t=6,p=2$%s"
bool(true)
Using password: string(44) "%s"
Hash: string(96) "$argon2id$v=19$m=4096,t=6,p=4$%s"
bool(true)
Using password: string(44) "%s"
Hash: string(96) "$argon2id$v=19$m=4096,t=6,p=8$%s"
bool(true)
Using password: string(44) "%s"
Hash: string(97) "$argon2id$v=19$m=4096,t=12,p=2$%s"
bool(true)
Using password: string(44) "%s"
Hash: string(97) "$argon2id$v=19$m=4096,t=12,p=4$%s"
bool(true)
Using password: string(44) "%s"
Hash: string(97) "$argon2id$v=19$m=4096,t=12,p=8$%s"
bool(true)
