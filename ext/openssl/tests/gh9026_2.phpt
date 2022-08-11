--TEST--
Bug GH-9026: openssl_encrypt() passphrase too long with error handler
--EXTENSIONS--
openssl
--FILE--
<?php

function err_handler($errno, $errstr, $errfile, $errline) {
    throw new Exception($errstr);
}

set_error_handler("err_handler", E_DEPRECATED);

$cipher = "aes-128-ctr";
$data = "test";
$passphrase = "KeyLengthIs16_123";
$iv = str_repeat("\x00", openssl_cipher_iv_length($cipher));
$flags = 0;

try {
    var_dump(openssl_encrypt(
        $data,
        $cipher,
        $passphrase,
        $flags,
        $iv
    ));
} catch (Exception $e) {
    echo $e->getMessage();
}

?>
--EXPECT--
openssl_encrypt(): Passphrase is too long and will be truncated to 16 characters
