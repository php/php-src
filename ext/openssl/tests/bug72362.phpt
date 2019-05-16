--TEST--
Bug #72362: OpenSSL Blowfish encryption is incorrect for short keys
--SKIPIF--
<?php
if (!extension_loaded("openssl")) die("skip openssl not loaded");
?>
--FILE--
<?php
var_dump(
    bin2hex(
        openssl_encrypt(
            "this is a test string",
            "bf-ecb",
            "12345678",
            OPENSSL_RAW_DATA | OPENSSL_DONT_ZERO_PAD_KEY
        )
    )
);
var_dump(
    bin2hex(
        openssl_encrypt(
            "this is a test string",
            "bf-ecb",
            "1234567812345678",
            OPENSSL_RAW_DATA
        )
    )
);
?>
--EXPECT--
string(48) "e3214d1b16e574828c8a3e222202dde81afd1ad2cb165ab3"
string(48) "e3214d1b16e574828c8a3e222202dde81afd1ad2cb165ab3"
