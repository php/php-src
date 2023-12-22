--TEST--
openssl_csr_sign() with serial and serial_hex parameters
--EXTENSIONS--
openssl
--FILE--
<?php
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

var_dump($cert1 = openssl_csr_sign($csr, null, $privkey, 365, $args, 1234567));
var_dump($cert2 = openssl_csr_sign($csr, null, $privkey, 365, $args, serial_hex: 'DEADBEEF'));
var_dump($cert3 = openssl_csr_sign($csr, null, $privkey, 365, $args, 10, 'DEADBEEFDEADBEEFDEADBEEFDEADBEEFDEADBEEF'));

var_dump(openssl_csr_sign($csr, null, $privkey, 365, $args, 0, 'DEADBEEG'));
var_dump(openssl_csr_sign($csr, null, $privkey, 365, $args, 0, '0xDEADBEEF'));
var_dump(openssl_csr_sign($csr, null, $privkey, 365, $args, 0, str_repeat('FF', 500)));

var_dump(openssl_x509_parse($cert1)['serialNumber']);
var_dump(openssl_x509_parse($cert1)['serialNumberHex']);
var_dump(openssl_x509_parse($cert2)['serialNumber']);
var_dump(openssl_x509_parse($cert2)['serialNumberHex']);
var_dump(openssl_x509_parse($cert3)['serialNumber']);
var_dump(openssl_x509_parse($cert3)['serialNumberHex']);
?>
--EXPECTF--
object(OpenSSLCertificate)#%d (0) {
}
object(OpenSSLCertificate)#%d (0) {
}
object(OpenSSLCertificate)#%d (0) {
}

Warning: openssl_csr_sign(): Error parsing serial number in %s on line %d
bool(false)

Warning: openssl_csr_sign(): Error parsing serial number in %s on line %d
bool(false)

Warning: openssl_csr_sign(): Error parsing serial number because it is too long in %s on line %d
bool(false)
string(7) "1234567"
string(6) "12D687"
string(10) "3735928559"
string(8) "DEADBEEF"
string(42) "0xDEADBEEFDEADBEEFDEADBEEFDEADBEEFDEADBEEF"
string(40) "DEADBEEFDEADBEEFDEADBEEFDEADBEEFDEADBEEF"