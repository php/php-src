--TEST--
Request #70438: Add IV parameter for openssl_seal and openssl_open
--EXTENSIONS--
openssl
--SKIPIF--
<?php
if (!in_array('AES-128-CBC', openssl_get_cipher_methods(true))) {
    print "skip";
}
?>
--FILE--
<?php
$data = "openssl_seal() test";
$cipher = 'AES-128-CBC';
$pub_key = "file://" . __DIR__ . "/public.key";
$priv_key = "file://" . __DIR__ . "/private_rsa_1024.key";

try {
    openssl_seal($data, $sealed, $ekeys, array($pub_key, $pub_key), $cipher);
} catch (\ValueError $e) {
    echo $e->getMessage() . \PHP_EOL;
}

openssl_seal($data, $sealed, $ekeys, array($pub_key, $pub_key), 'sparkles', $iv);
openssl_seal($data, $sealed, $ekeys, array($pub_key, $pub_key), $cipher, $iv);
openssl_open($sealed, $decrypted, $ekeys[0], $priv_key, $cipher, $iv);
echo $decrypted;
?>
--EXPECTF--
openssl_seal(): Argument #6 ($iv) cannot be null for the chosen cipher algorithm

Warning: openssl_seal(): Unknown cipher algorithm in %s on line %d
openssl_seal() test
