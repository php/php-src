--TEST--
openssl_csr_sign() tests
--SKIPIF--
<?php if (!extension_loaded("openssl")) print "skip"; ?>
--FILE--
<?php
$cert = "file://" . __DIR__ . "/cert.crt";
$priv = "file://" . __DIR__ . "/private_rsa_1024.key";
$wrong = "wrong";
$pub = "file://" . __DIR__ . "/public.key";
$config = __DIR__ . DIRECTORY_SEPARATOR . 'openssl.cnf';
$config_arg = array('config' => $config);

$dn = array(
    "countryName" => "BR",
    "stateOrProvinceName" => "Rio Grande do Sul",
    "localityName" => "Porto Alegre",
    "commonName" => "Henrique do N. Angelo",
    "emailAddress" => "hnangelo@php.net"
);

$args = array(
    "digest_alg" => "sha1",
    "private_key_bits" => 2048,
    "private_key_type" => OPENSSL_KEYTYPE_DSA,
    "encrypt_key" => true,
    "config" => $config
);

$privkey = openssl_pkey_new($config_arg);
$csr = openssl_csr_new($dn, $privkey, $args);
var_dump(openssl_csr_sign($csr, null, $privkey, 365, $args));
var_dump(openssl_csr_sign($csr, null, $privkey, 365, $config_arg));
var_dump(openssl_csr_sign($csr, $cert, $priv, 365, $config_arg));
var_dump(openssl_csr_sign($csr, openssl_x509_read($cert), $priv, 365, $config_arg));
var_dump(openssl_csr_sign($csr, $wrong, $privkey, 365));
var_dump(openssl_csr_sign($csr, null, $wrong, 365));
var_dump(openssl_csr_sign($csr, null, $privkey, $wrong));
var_dump(openssl_csr_sign($csr, null, $privkey, 365, $wrong));
var_dump(openssl_csr_sign($wrong, null, $privkey, 365));
var_dump(openssl_csr_sign(array(), null, $privkey, 365));
var_dump(openssl_csr_sign($csr, array(), $privkey, 365));
var_dump(openssl_csr_sign($csr, null, array(), 365));
var_dump(openssl_csr_sign($csr, null, $privkey, array()));
var_dump(openssl_csr_sign($csr, null, $privkey, 365, $config_arg));
?>
--EXPECTF--
resource(%d) of type (OpenSSL X.509)
resource(%d) of type (OpenSSL X.509)
resource(%d) of type (OpenSSL X.509)
resource(%d) of type (OpenSSL X.509)

Warning: openssl_csr_sign(): cannot get cert from parameter 2 in %s on line %d
bool(false)

Warning: openssl_csr_sign(): cannot get private key from parameter 3 in %s on line %d
bool(false)

Warning: openssl_csr_sign() expects parameter 4 to be int, string given in %s on line %d
NULL

Warning: openssl_csr_sign() expects parameter 5 to be array, string given in %s on line %d
NULL

Warning: openssl_csr_sign(): cannot get CSR from parameter 1 in %s on line %d
bool(false)

Warning: openssl_csr_sign(): cannot get CSR from parameter 1 in %s on line %d
bool(false)

Warning: openssl_csr_sign(): cannot get cert from parameter 2 in %s on line %d
bool(false)

Warning: openssl_csr_sign(): key array must be of the form array(0 => key, 1 => phrase) in %s on line %d

Warning: openssl_csr_sign(): cannot get private key from parameter 3 in %s on line %d
bool(false)

Warning: openssl_csr_sign() expects parameter 4 to be int, array given in %s on line %d
NULL
resource(%d) of type (OpenSSL X.509)
