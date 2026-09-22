--TEST--
Compatibility of password_hash (libargon2 / openssl)
--EXTENSIONS--
openssl
sodium
--SKIPIF--
<?php
if (PASSWORD_ARGON2_PROVIDER != "standard") {
    echo "skip - libargon2 not available";
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

        /* hash with libargon2 / verify with openssl */
        $hash = password_hash($pass, PASSWORD_ARGON2ID, $opts);
        var_dump(openssl_password_verify($algo, $pass, $hash));

        /* hash with openssl / verify with libargon2 */
        $hash = openssl_password_hash($algo, $pass, $opts);
        var_dump(password_verify($pass, $hash));
    }
}
?>
--EXPECT--
Argon2 provider: string(8) "standard"
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)

