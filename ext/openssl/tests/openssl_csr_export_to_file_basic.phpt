--TEST--
openssl_csr_export_to_file() tests
--EXTENSIONS--
openssl
--SKIPIF--
<?php
if (!defined("OPENSSL_KEYTYPE_DSA")) die("skip DSA disabled");
?>
--FILE--
<?php
$csrfile = __DIR__ . "/openssl_csr_export_to_file_csr.tmp";
$wrong = "wrong";
$config = __DIR__ . DIRECTORY_SEPARATOR . 'openssl.cnf';
$phex = 'dcf93a0b883972ec0e19989ac5a2ce310e1d37717e8d9571bb7623731866e61e' .
        'f75a2e27898b057f9891c2e27a639c3f29b60814581cd3b2ca3986d268370557' .
        '7d45c2e7e52dc81c7a171876e5cea74b1448bfdfaf18828efd2519f14e45e382' .
        '6634af1949e5b535cc829a483b8a76223e5d490a257f05bdff16f2fb22c583ab';
$dh_details = array('p' => $phex, 'g' => '2');
$dh = openssl_pkey_new(array(
    'dh'=> array('p' => hex2bin($phex), 'g' => '2'))
);

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

$privkey_file = 'file://' . __DIR__ . '/private_rsa_2048.key';
$csr = openssl_csr_new($dn, $privkey_file, $args);
var_dump(openssl_csr_export_to_file($csr, $csrfile));
var_dump(file_get_contents($csrfile));

var_dump(openssl_csr_export_to_file($wrong, $csrfile));

try {
    openssl_csr_export_to_file($dh, $csrfile);
} catch (TypeError $e) {
    echo $e->getMessage(), "\n";
}
var_dump(openssl_csr_export_to_file($csr, $csrfile, false));
?>
--CLEAN--
<?php
$csrfile = __DIR__ . "/openssl_csr_export_to_file_csr.tmp";
if (file_exists($csrfile)) {
    unlink($csrfile);
}
?>
--EXPECTF--
bool(true)
string(1086) "-----BEGIN CERTIFICATE REQUEST-----
MIIC6jCCAdICAQAwgaQxCzAJBgNVBAYTAkJSMRowGAYDVQQIExFSaW8gR3JhbmRl
IGRvIFN1bDEVMBMGA1UEBxMMUG9ydG8gQWxlZ3JlMR4wHAYDVQQDExVIZW5yaXF1
ZSBkbyBOLiBBbmdlbG8xHzAdBgkqhkiG9w0BCQEWEGhuYW5nZWxvQHBocC5uZXQx
ITAfBgNVBAoTGEludGVybmV0IFdpZGdpdHMgUHR5IEx0ZDCCASIwDQYJKoZIhvcN
AQEBBQADggEPADCCAQoCggEBAK21JlVtWPqyc2UQtw2AdJHSIL5OzAHWsu/Mmxjz
w4VZWwG+Ao1m2j7OJ8lAkxLfSlRtB+6Vx8Q21krAtve+M3b6pT9C9qKdfzNkrXk3
BggTncBh9ozkVQGS/P1m0zn/SKSgDO+6DdeLHLMjpUASaoYfsay4PJLAdnTqLOeM
g6qNE6u0ebZXVfmpSmV1pSZ6kQnxbsb6rX1iOZxkwHnVWYb40Hy0EILo31x6BVqB
m159m7s38ChiRHqlj20DmRfxXjiT5YDgYYQ29wQBTVQrTN5O9UW5Y+eKTXd8r6te
dsbIBXdKN7NeX7ksGYHq1I3hLsP8EyvZO78qfjKyEB0Jj3UCAwEAAaAAMA0GCSqG
SIb3DQEBCwUAA4IBAQCNtCIfMHBDRvNqHmrDfR/+A7ZJ+n/XzA2uQhvjEq91DeT8
IE7gjUtmj2sqKmHGIDO4uN4F9ZHYzcNk23n6CMljYqJLbB2dHC0V6vkDB7qod1TH
/SK39Yj0ji2AT45LD5rLH3vd1bjxdwwhyPyGhshKOIdnmBv4mwTRANIsiISMQV4Y
ZPAXJ5DTKkgdsY14hqhyWct1bWMPpj2MCLQGjKxK8vmbiKaNL1XxAS7chTXoy7un
NvBKc82Wy3XEuC9AkNFEytD6kA9gu8nFydvYTOvvhaQrf9RzwSitgi9Vj3mbujsN
f1JMPX0/eHrKvG9wBZu28FdS54xoWGeD1NGraW24
-----END CERTIFICATE REQUEST-----
"

Warning: openssl_csr_export_to_file(): X.509 Certificate Signing Request cannot be retrieved in %s on line %d
bool(false)
openssl_csr_export_to_file(): Argument #1 ($csr) must be of type OpenSSLCertificateSigningRequest|string, OpenSSLAsymmetricKey given
bool(true)
