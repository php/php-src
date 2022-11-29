--TEST--
openssl_csr_sign() tests
--EXTENSIONS--
openssl
--SKIPIF--
<?php
if (!defined("OPENSSL_KEYTYPE_DSA")) die("skip DSA disabled");
?>
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
    "digest_alg" => "sha256",
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
var_dump(openssl_csr_sign($wrong, null, $privkey, 365));

try {
    openssl_csr_sign(array(), null, $privkey, 365);
} catch (TypeError $exception) {
    echo $exception->getMessage() . "\n";
}

try {
    var_dump(openssl_csr_sign($csr, array(), $privkey, 365));
} catch (TypeError $exception) {
    echo $exception->getMessage() . "\n";
}

try {
    var_dump(openssl_csr_sign($csr, null, array(), 365));
} catch (ValueError $exception) {
    echo $exception->getMessage() . "\n";
}
var_dump(openssl_csr_sign($csr, null, $privkey, 365, $config_arg));
?>
--EXPECTF--
object(OpenSSLCertificate)#%d (0) {
}
object(OpenSSLCertificate)#%d (0) {
}
object(OpenSSLCertificate)#%d (0) {
}
object(OpenSSLCertificate)#%d (0) {
}

Warning: openssl_csr_sign(): X.509 Certificate cannot be retrieved in %s on line %d
bool(false)

Warning: openssl_csr_sign(): Cannot get private key from parameter 3 in %s on line %d
bool(false)

Warning: openssl_csr_sign(): X.509 Certificate Signing Request cannot be retrieved in %s on line %d
bool(false)
openssl_csr_sign(): Argument #1 ($csr) must be of type OpenSSLCertificateSigningRequest|string, array given
openssl_csr_sign(): Argument #2 ($ca_certificate) must be of type OpenSSLCertificate|string|null, array given
Key array must be of the form array(0 => key, 1 => phrase)
object(OpenSSLCertificate)#%d (0) {
}
