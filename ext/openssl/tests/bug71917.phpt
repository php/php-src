--TEST--
Bug #71917: openssl_open() returns junk on envelope < 16 bytes
--EXTENSIONS--
openssl
--SKIPIF--
<?php
if (!in_array('rc4', openssl_get_cipher_methods())) die('skip rc4 not available');
?>
--FILE--
<?php
function test($envkey) {
    $publicKey = "file://" . __DIR__ . "/public.key";
    $privateKey = "file://" . __DIR__ . "/private_rsa_1024.key";
    openssl_public_encrypt($envkey, $envelope, $publicKey);
    $sealed = openssl_encrypt(
        'plaintext',
        'rc4', $envkey,
        OPENSSL_RAW_DATA | OPENSSL_DONT_ZERO_PAD_KEY
    );
    openssl_open($sealed, $output, $envelope, $privateKey, 'rc4');
    var_dump($output === 'plaintext');
}

// works - key of 16 bytes
test('1234567890123456i');
// fails - key of 15 bytes
test('123456789012345');
?>
--EXPECT--
bool(true)
bool(true)
