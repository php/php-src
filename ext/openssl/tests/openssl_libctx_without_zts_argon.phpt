--TEST--
openssl.libctx INI setting when Argon2 disable or ZTS not used
--EXTENSIONS--
openssl
--INI--
openssl.libctx = default
--SKIPIF--
<?php
if (ZEND_THREAD_SAFE && function_exists('openssl_password_hash')) {
    die("skip - ZTS test with Argon2 enabled");
}
?>
--FILE--
<?php
var_dump(ini_get('openssl.libctx'));
?>
--EXPECT--
string(7) "default"
