--TEST--
Bug #73711: Segfault in openssl_pkey_new when generating DSA or DH key
--EXTENSIONS--
openssl
--FILE--
<?php
$cnf = __DIR__ . DIRECTORY_SEPARATOR . 'bug73711.cnf';
var_dump(openssl_pkey_new(["private_key_type" => OPENSSL_KEYTYPE_DSA, 'config' => $cnf]));
var_dump(openssl_pkey_new(["private_key_type" => OPENSSL_KEYTYPE_DH, 'config' => $cnf]));
echo "DONE";
?>
--EXPECTF--
object(OpenSSLAsymmetricKey)#%d (0) {
}
object(OpenSSLAsymmetricKey)#%d (0) {
}
DONE
