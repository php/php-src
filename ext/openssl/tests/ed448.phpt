--TEST--
openssl_*() with OPENSSL_KEYTYPE_ED448
--EXTENSIONS--
openssl
--SKIPIF--
<?php
if (!defined("OPENSSL_KEYTYPE_ED448")) die("skip ED448 not available");
?>
--FILE--
<?php
echo "Testing openssl_pkey_new\n";
$key1 = openssl_pkey_new([ "private_key_type" => OPENSSL_KEYTYPE_ED448 ]);
var_dump($key1);

$d1 = openssl_pkey_get_details($key1);
var_dump($d1["bits"] === 456);
var_dump(strlen($d1["key"]) === 146);
var_dump(strlen($d1["ed448"]["priv_key"]) === 57);
var_dump($d1["type"] === OPENSSL_KEYTYPE_ED448);

$key2 = openssl_pkey_new($d1);
var_dump($key2);

$d2 = openssl_pkey_get_details($key2);
// Compare array
var_dump($d1 == $d2);

// Check that the public key info is computed from the private key if it is missing.
$d1_priv = $d1;
unset($d1_priv["ed448"]["pub_key"]);
$key3 = openssl_pkey_new($d1_priv);
var_dump($key3);
$d3 = openssl_pkey_get_details($key3);
var_dump($d1 == $d3);

// create public key from private
$d1_pub = $d1;
unset($d1_pub["ed448"]["priv_key"]);
$pubkey1 = openssl_pkey_new($d1_pub);
var_dump($pubkey1);
$pubkey1_d = openssl_pkey_get_details($pubkey1);
var_dump($d1_pub == $pubkey1_d);

// sign and verify
echo "Testing openssl_sign and openssl_verify\n";
$payload = "somedata";
var_dump(openssl_sign($payload, $signature, $key1, 0));
var_dump(strlen($signature) === 114);
var_dump(openssl_verify($payload, $signature, $pubkey1, 0));


$dn = array(
    "countryName" => "BR",
    "stateOrProvinceName" => "Rio Grande do Sul",
    "localityName" => "Porto Alegre",
    "commonName" => "Henrique do N. Angelo",
    "emailAddress" => "hnangelo@php.net"
);
$config =  __DIR__ . DIRECTORY_SEPARATOR . 'openssl.cnf';
$args = array(
    "private_key_type" => OPENSSL_KEYTYPE_ED448,
    "config" => $config,
    "digest_alg" => "null",
);

// openssl_csr_new creates a new public key pair if the key argument is null
echo "Testing openssl_csr_new with key generation\n";
$keyGenerate = null;
$csr = openssl_csr_new($dn, $keyGenerate, $args);
var_dump($keyGenerate);
var_dump($csr);

echo "Testing openssl_csr_new with existing key\n";
$csr = openssl_csr_new($dn, $key1, $args);
$pubkey_csr = openssl_pkey_get_details(openssl_csr_get_public_key($csr));
var_dump($pubkey_csr == $pubkey1_d);

echo "Testing openssl_csr_sign\n";
$x509 = openssl_csr_sign($csr, null, $key1, 365, $args);
var_dump($x509);

echo "Testing openssl_x509_{verify,check_private_key}\n";
var_dump(openssl_x509_check_private_key($x509, $key1));
var_dump(openssl_x509_verify($x509, $pubkey1));
var_dump(openssl_x509_check_private_key($x509, $keyGenerate));

?>
--EXPECTF--
Testing openssl_pkey_new
object(OpenSSLAsymmetricKey)#%d (0) {
}
bool(true)
bool(true)
bool(true)
bool(true)
object(OpenSSLAsymmetricKey)#%d (0) {
}
bool(true)
object(OpenSSLAsymmetricKey)#%d (0) {
}
bool(true)
object(OpenSSLAsymmetricKey)#%d (0) {
}
bool(true)
Testing openssl_sign and openssl_verify
bool(true)
bool(true)
int(1)
Testing openssl_csr_new with key generation
object(OpenSSLAsymmetricKey)#%d (0) {
}
object(OpenSSLCertificateSigningRequest)#%d (0) {
}
Testing openssl_csr_new with existing key
bool(true)
Testing openssl_csr_sign
object(OpenSSLCertificate)#%d (0) {
}
Testing openssl_x509_{verify,check_private_key}
bool(true)
int(1)
bool(false)
