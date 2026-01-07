--TEST--
openssl: AES-256-SIV AEAD tag and AAD roundtrip
--EXTENSIONS--
openssl
--FILE--
<?php
$algo = 'aes-256-siv';
$key = str_repeat('1', 64);
$tag = '';
$aad = null;
$input = 'Hello world!';

$ciphertext = openssl_encrypt(
    'Hello world!',
    $algo,
    $key,
    OPENSSL_RAW_DATA,
    '', // IV is empty for this cipher in PHP
    $tag, // gets filled with the SIV
    $aad,
    16
);

echo 'input: ' . $input . PHP_EOL;
echo 'tag: ' . bin2hex($tag) . PHP_EOL;
echo 'ciphertext: ' . bin2hex($ciphertext) . PHP_EOL;
echo 'combined: ' . bin2hex($tag . $ciphertext) . PHP_EOL;

$dec = openssl_decrypt(
    $ciphertext,
    $algo,
    $key,
    OPENSSL_RAW_DATA,
    '',
    $tag,
    $aad
);

echo 'decrypted: ' . var_export($dec, true) . PHP_EOL;
?>
--EXPECTF--
input: Hello world!
tag: c06f0df087e2784c5560ce5d0b378311
ciphertext: 72fffba74d7bc3ddcceeb6d1
combined: c06f0df087e2784c5560ce5d0b37831172fffba74d7bc3ddcceeb6d1
decrypted: 'Hello world!'

