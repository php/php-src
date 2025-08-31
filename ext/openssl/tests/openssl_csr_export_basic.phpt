--TEST--
openssl_csr_export() tests
--EXTENSIONS--
openssl
--SKIPIF--
<?php
if (!defined("OPENSSL_KEYTYPE_DSA")) die("skip DSA disabled");
?>
--FILE--
<?php
$wrong = "wrong";
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
    "digest_alg" => "sha256",
    "private_key_bits" => 2048,
    "private_key_type" => OPENSSL_KEYTYPE_DSA,
    "encrypt_key" => true,
    "config" => $config,
);

$privkey = openssl_pkey_new($config_arg);
$csr = openssl_csr_new($dn, $privkey, $args);
var_dump(openssl_csr_export($csr, $output));
try {
    var_dump(openssl_csr_export($wrong, $output));
} catch (TypeError $e) {
    echo $e->getMessage(), "\n";
}
try {
    var_dump(openssl_csr_export($privkey, $output));
} catch (TypeError $e) {
    echo $e->getMessage(), "\n";
}
var_dump(openssl_csr_export($csr, $output, false));
?>
--EXPECTF--
bool(true)

Warning: openssl_csr_export(): X.509 Certificate Signing Request cannot be retrieved in %s on line %d
bool(false)
openssl_csr_export(): Argument #1 ($csr) must be of type OpenSSLCertificateSigningRequest|string, OpenSSLAsymmetricKey given
bool(true)
