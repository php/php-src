--TEST--
Bug #73833: null character not allowed in openssl_pkey_get_private
--SKIPIF--
<?php
if (!extension_loaded("openssl")) die("skip openssl not loaded");
?>
--FILE--
<?php
$passwords = ["abc\x00defghijkl", "abcdefghikjl"];
$conf = ['config' => __DIR__ . DIRECTORY_SEPARATOR . 'openssl.cnf'];

foreach($passwords as $password) {
    $key = openssl_pkey_new($conf);

    if (openssl_pkey_export($key, $privatePEM, $password, $conf) === false) {
        echo "Failed to encrypt.\n";
    } else {
        echo "Encrypted!\n";
    }
    if (openssl_pkey_get_private($privatePEM, $password) === false) {
        echo "Failed to decrypt.\n";
    } else {
        echo "Decrypted!\n";
    }
}
?>
--EXPECT--
Encrypted!
Decrypted!
Encrypted!
Decrypted!
