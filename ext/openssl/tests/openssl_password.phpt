--TEST--
Basic features of password_hash
--EXTENSIONS--
openssl
--SKIPIF--
<?php
if (!function_exists('openssl_password_hash')) {
    echo "skip - No openssl_password_hash";
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
        foreach(['argon2i', 'argon2id'] as $algo) {
            $pass = "secret$mem$time$algo";
            $hash = openssl_password_hash($algo, $pass, $opts);
            var_dump(openssl_password_verify($algo, $pass, $hash));
        }
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

