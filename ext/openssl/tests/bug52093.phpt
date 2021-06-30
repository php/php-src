--TEST--
Bug #52093 (openssl_csr_sign silently truncates $serial)
--SKIPIF--
<?php if (!extension_loaded("openssl")) print "skip"; ?>
--FILE--
<?php
$dn = array(
    "countryName" => "BR",
    "stateOrProvinceName" => "Rio Grande do Sul",
    "localityName" => "Porto Alegre",
    "commonName" => "Henrique do N. Angelo",
    "emailAddress" => "hnangelo@php.net"
);

$privkey = openssl_pkey_new();
$csr = openssl_csr_new($dn, $privkey);
var_dump(openssl_csr_sign($csr, null, $privkey, 365, [], PHP_INT_MAX));
?>
--EXPECTF--
Warning: openssl_csr_sign(): serial out of range, will be truncated in %s on line %d
resource(6) of type (OpenSSL X.509)
