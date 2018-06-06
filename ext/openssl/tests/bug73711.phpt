--TEST--
Bug #73711: Segfault in openssl_pkey_new when generating DSA or DH key
--SKIPIF--
<?php
if (!extension_loaded("openssl")) die("skip openssl not loaded");
?>
--FILE--
<?php
$cnf = dirname(__FILE__) . DIRECTORY_SEPARATOR . 'bug73711.cnf';
var_dump(openssl_pkey_new(["private_key_type" => OPENSSL_KEYTYPE_DSA, 'config' => $cnf]));
var_dump(openssl_pkey_new(["private_key_type" => OPENSSL_KEYTYPE_DH, 'config' => $cnf]));
echo "DONE";
?>
--EXPECTF--
resource(%d) of type (OpenSSL key)
resource(%d) of type (OpenSSL key)
DONE
