--TEST--
Bug #73711: Segfault in openssl_pkey_new when generating DSA or DH key
--EXTENSIONS--
openssl
--SKIPIF--
<?php
if (!defined("OPENSSL_KEYTYPE_DSA")) die("skip DSA disabled");
?>
--FILE--
<?php
$config = __DIR__ . DIRECTORY_SEPARATOR . 'openssl.cnf';
var_dump(openssl_pkey_new([
    "private_key_type" => OPENSSL_KEYTYPE_DSA,
    "private_key_bits" => 1024,
    'config' => $config,
]));
var_dump(openssl_pkey_new([
    "private_key_type" => OPENSSL_KEYTYPE_DH,
    "private_key_bits" => 512,
    'config' => $config,
]));
echo "DONE";
?>
--EXPECTF--
object(OpenSSLAsymmetricKey)#%d (0) {
}
object(OpenSSLAsymmetricKey)#%d (0) {
}
DONE
