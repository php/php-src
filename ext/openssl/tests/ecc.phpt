--TEST--
openssl_*() with OPENSSL_KEYTYPE_EC
--EXTENSIONS--
openssl
--SKIPIF--
<?php
if (!defined("OPENSSL_KEYTYPE_EC")) die("skip EC disabled");
?>
--FILE--
<?php
$config =  __DIR__ . DIRECTORY_SEPARATOR . 'openssl.cnf';
$args = array(
    "curve_name" => "secp384r1",
    "private_key_type" => OPENSSL_KEYTYPE_EC,
    "config" => $config,
);
echo "Testing openssl_pkey_new\n";
$key1 = openssl_pkey_new($args);
var_dump($key1);

$argsFailed = array(
    "curve_name" => "invalid_cuve_name",
    "private_key_type" => OPENSSL_KEYTYPE_EC,
    "config" => $config,
);

$keyFailed = openssl_pkey_new($argsFailed);
var_dump($keyFailed);

$d1 = openssl_pkey_get_details($key1);
var_dump($d1["bits"]);
var_dump(strlen($d1["key"]));
var_dump($d1["ec"]["curve_name"]);
var_dump($d1["type"] == OPENSSL_KEYTYPE_EC);

$key2 = openssl_pkey_new($d1);
var_dump($key2);

$d2 = openssl_pkey_get_details($key2);
// Compare array
var_dump($d1 === $d2);

// Check that the public key info is computed from the private key if it is missing.
$d1_priv = $d1;
unset($d1_priv["ec"]["x"]);
unset($d1_priv["ec"]["y"]);

$key3 = openssl_pkey_new($d1_priv);
var_dump($key3);
$d3 = openssl_pkey_get_details($key3);
var_dump($d1 === $d3);

$dn = array(
    "countryName" => "BR",
    "stateOrProvinceName" => "Rio Grande do Sul",
    "localityName" => "Porto Alegre",
    "commonName" => "Henrique do N. Angelo",
    "emailAddress" => "hnangelo@php.net"
);

// openssl_csr_new creates a new public key pair if the key argument is null
echo "Testing openssl_csr_new with key generation\n";
$keyGenerate = null;
var_dump($keyGenerate);
$csr = openssl_csr_new($dn, $keyGenerate, $args);

var_dump($keyGenerate);

$args["digest_alg"] = "sha256";
echo "Testing openssl_csr_new with existing ecc key\n";
$csr = openssl_csr_new($dn, $key1, $args);
var_dump($csr);

$pubkey1 = openssl_pkey_get_details(openssl_csr_get_public_key($csr));
var_dump(isset($pubkey1["ec"]["priv_key"]));
unset($d1["ec"]["priv_key"]);
$diff = array_diff($d1["ec"], $pubkey1["ec"]);
var_dump(isset($diff["d"]) && is_string($diff["d"]) && strlen($diff["d"]) > 0);

$x509 = openssl_csr_sign($csr, null, $key1, 365, $args);
var_dump($x509);

echo "Testing openssl_x509_check_private_key\n";
var_dump(openssl_x509_check_private_key($x509, $key1));

$key3 = openssl_pkey_new($args);
var_dump(openssl_x509_check_private_key($x509, $key3));

echo "Testing openssl_get_curve_names\n";
$curve_names = openssl_get_curve_names();

var_dump(is_array($curve_names));

foreach ($curve_names as $curve_name) {
    if ("secp384r1" === $curve_name) {
        echo "Found secp384r1 in curve names\n";
    }
}
?>
--EXPECTF--
Testing openssl_pkey_new
object(OpenSSLAsymmetricKey)#1 (0) {
}

Warning: openssl_pkey_new(): Unknown elliptic curve (short) name invalid_cuve_name in %s on line %d
bool(false)
int(384)
int(215)
string(9) "secp384r1"
bool(true)
object(OpenSSLAsymmetricKey)#%d (0) {
}
bool(true)
object(OpenSSLAsymmetricKey)#%d (0) {
}
bool(true)
Testing openssl_csr_new with key generation
NULL
object(OpenSSLAsymmetricKey)#%d (0) {
}
Testing openssl_csr_new with existing ecc key
object(OpenSSLCertificateSigningRequest)#%d (0) {
}
bool(false)
bool(true)
object(OpenSSLCertificate)#%d (0) {
}
Testing openssl_x509_check_private_key
bool(true)
bool(false)
Testing openssl_get_curve_names
bool(true)
Found secp384r1 in curve names
