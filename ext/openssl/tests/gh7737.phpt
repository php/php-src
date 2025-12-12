--TEST--
GitHub Bug#7737 - openssl_seal/open() does not handle ciphers with Tags (e.g. AES-256-CGM)
--EXTENSIONS--
openssl
--SKIPIF--
<?php
// Skip if aes-256-cgm is not available in this build.
in_array('aes-256-gcm', openssl_get_cipher_methods()) or print 'skip';
?>
--FILE--
<?php

const CIPHER_ALGO = 'aes-256-gcm';
const KEY_TYPE = OPENSSL_KEYTYPE_RSA;
const PLAINTEXT = 'Test Data String';

(function() {
    $key = openssl_pkey_new(['type' => KEY_TYPE]);
    define('KEY_PUBLIC', openssl_pkey_get_details($key)['key']);
    define('KEY_PRIVATE', openssl_pkey_get_private($key));
})();

echo 'Plaintext: '; var_dump(PLAINTEXT);

$sealResult = openssl_seal(PLAINTEXT,
                           $sealedData, /* out */
                           $sealedKeys, /* out */
                           [KEY_PUBLIC],
                           CIPHER_ALGO,
                           $iv, /* out */
                           $tag); /* out */

echo 'Seal Result: '; var_dump($sealResult);
echo 'Sealed Data: '; var_dump(strlen($sealedData));
echo 'IV Length: '; var_dump(strlen($iv));
echo 'Tag Length: '; var_dump(strlen($tag));

$unsealResult = openssl_open($sealedData,
                             $unsealedData, /* out */
                             $sealedKeys[0],
                             KEY_PRIVATE,
                             CIPHER_ALGO,
                             $iv,
                             $tag);

echo 'Unseal Result: '; var_dump($unsealResult);
echo 'Unsealed Data: '; var_dump($unsealedData);

?>
--EXPECT--
Plaintext: string(16) "Test Data String"
Seal Result: int(16)
Sealed Data: int(16)
IV Length: int(12)
Tag Length: int(16)
Unseal Result: bool(true)
Unsealed Data: string(16) "Test Data String"
