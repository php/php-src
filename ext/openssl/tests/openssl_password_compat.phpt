--TEST--
Compatibility of password_hash (libsodium / openssl)
--EXTENSIONS--
openssl
sodium
--SKIPIF--
<?php
if (!function_exists('sodium_crypto_pwhash_str_verify')) {
    echo "skip - No crypto_pwhash_str_verify";
}

if (!function_exists('openssl_password_hash')) {
    echo "skip - No crypto_pwhash_str_verify";
}
?>
--FILE--
<?php

echo 'Argon2 provider: ';
var_dump(PASSWORD_ARGON2_PROVIDER);

foreach([1, 2] as $mem) {
    foreach([1, 2] as $time) {
        $opts = [
            'memory_cost' => PASSWORD_ARGON2_DEFAULT_MEMORY_COST / $mem,
            'time_cost'   => PASSWORD_ARGON2_DEFAULT_TIME_COST / $time,
            'threads'     => PASSWORD_ARGON2_DEFAULT_THREADS,
        ];
        $algo = 'argon2id';
        $pass = "secret$mem$time$algo";

        /* hash with libsodium / verify with openssl */
        $hash = sodium_crypto_pwhash_str($pass, PASSWORD_ARGON2_DEFAULT_TIME_COST / $time, PASSWORD_ARGON2_DEFAULT_MEMORY_COST / $mem);
        var_dump(openssl_password_verify($algo, $pass, $hash));

        /* hash with openssl / verify with libsodium */
        $hash = openssl_password_hash($algo, $pass, $opts);
        var_dump(sodium_crypto_pwhash_str_verify($hash, $pass));
    }
}
?>
--EXPECTF--
Argon2 provider: string(%d) "%s"
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)

