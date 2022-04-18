--TEST--
Bug #52093 (openssl_csr_sign truncates $serial)
--EXTENSIONS--
openssl
--SKIPIF--
<?php
if (PHP_INT_SIZE !== 8) die("skip this test is for 64bit platforms only");
?>
--FILE--
<?php
$dn = array(
    "countryName" => "BR",
    "stateOrProvinceName" => "Rio Grande do Sul",
    "localityName" => "Porto Alegre",
    "commonName" => "Henrique do N. Angelo",
    "emailAddress" => "hnangelo@php.net"
);
$options = ['config' => __DIR__ . DIRECTORY_SEPARATOR . 'openssl.cnf'];
$privkey = openssl_pkey_new();
$csr = openssl_csr_new($dn, $privkey, $options);
$cert = openssl_csr_sign($csr, null, $privkey, 365, $options, PHP_INT_MAX);
var_dump(openssl_x509_parse($cert)['serialNumber']);
?>
--EXPECT--
string(19) "9223372036854775807"
