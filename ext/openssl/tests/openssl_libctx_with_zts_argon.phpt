--TEST--
openssl.libctx INI setting when Argon2 enabled and ZTS used
--EXTENSIONS--
openssl
--INI--
openssl.libctx = default
--SKIPIF--
<?php
if (!ZEND_THREAD_SAFE) {
    die("skip - Non ZTS test");
}
if (!function_exists('openssl_password_hash')) {
    die("skip - OpenSSL Argon2 not enabled");
}
?>
--FILE--
<?php
var_dump(ini_get('openssl.libctx'));
?>
--EXPECT--
Fatal error: PHP Startup: OpenSSL libctx "default" cannot be used in this configuration in Unknown on line 0
string(6) "custom"
