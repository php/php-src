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
var_dump(array_diff($d1["ec"], $pubkey1["ec"]));

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

// EC - generate keypair with curve_name
echo "Testing openssl_pkey_new with ec curve_name\n";
$curve_name = openssl_get_curve_names()[0];

$ec = openssl_pkey_new(array(
    'ec'=> array(
        'curve_name' => $curve_name,
    )
));

var_dump($ec);
$details = openssl_pkey_get_details($ec);
$ec_details = $details['ec'];
var_dump($ec_details['curve_name'] === $curve_name);

// EC - generate keypair from priv_key "d" with custom params (ECDSA curve)
echo "Testing openssl_pkey_new with ec custom params\n";
$d = hex2bin('8D0AC65AAEA0D6B96254C65817D4A143A9E7A03876F1A37D');
$x = hex2bin('98E07AAD50C31F9189EBE6B8B5C70E5DEE59D7A8BC344CC6');
$y = hex2bin('6109D3D96E52D0867B9D05D72D07BE5876A3D973E0E96792');
$p = hex2bin('BDB6F4FE3E8B1D9E0DA8C0D46F4C318CEFE4AFE3B6B8551F');
$a = hex2bin('BB8E5E8FBC115E139FE6A814FE48AAA6F0ADA1AA5DF91985');
$b = hex2bin('1854BEBDC31B21B7AEFC80AB0ECD10D5B1B3308E6DBF11C1');
$g_x = hex2bin('4AD5F7048DE709AD51236DE65E4D4B482C836DC6E4106640');
$g_y = hex2bin('02BB3A02D4AAADACAE24817A4CA3A1B014B5270432DB27D2');
$order = hex2bin('BDB6F4FE3E8B1D9E0DA8C0D40FC962195DFAE76F56564677');

$ec = openssl_pkey_new(array(
    'ec'=> array(
        'p' => $p,
        'a' => $a,
        'b' => $b,
        'order' => $order,
        'g_x' => $g_x,
        'g_y' => $g_y,
        'd' => $d,
    )
));

$details = openssl_pkey_get_details($ec);
$ec_details = $details['ec'];
var_dump($ec_details['x'] === $x);
var_dump($ec_details['y'] === $y);
var_dump($ec_details['d'] === $d);

echo "Testing openssl_pkey_new with ec missing params \n";
// EC - missing all params
$ec = openssl_pkey_new(array(
    'ec'=> array()
));

// EC - missing "p" param
$ec = openssl_pkey_new(array(
    'ec'=> array(
        'a' => $a,
        'b' => $b,
        'order' => $order
    )
));

// EC - missing "a" param
$ec = openssl_pkey_new(array(
    'ec'=> array(
        'p' => $p,
        'b' => $b,
        'order' => $order
    )
));

// EC - missing "b" param
$ec = openssl_pkey_new(array(
    'ec'=> array(
        'p' => $p,
        'a' => $a,
        'order' => $order
    )
));

// EC - missing "order" param
$ec = openssl_pkey_new(array(
    'ec'=> array(
        'p' => $p,
        'a' => $a,
        'b' => $b
    )
));
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
array(1) {
  ["d"]=>
  string(%d) "%a"
}
object(OpenSSLCertificate)#%d (0) {
}
Testing openssl_x509_check_private_key
bool(true)
bool(false)
Testing openssl_get_curve_names
bool(true)
Found secp384r1 in curve names
Testing openssl_pkey_new with ec curve_name
object(OpenSSLAsymmetricKey)#%d (0) {
}
bool(true)
Testing openssl_pkey_new with ec custom params
bool(true)
bool(true)
bool(true)
Testing openssl_pkey_new with ec missing params 

Warning: openssl_pkey_new(): Missing params: curve_name in %s on line %d

Warning: openssl_pkey_new(): Missing params: curve_name or p, a, b, order in %s on line %d

Warning: openssl_pkey_new(): Missing params: curve_name or p, a, b, order in %s on line %d

Warning: openssl_pkey_new(): Missing params: curve_name or p, a, b, order in %s on line %d

Warning: openssl_pkey_new(): Missing params: curve_name or p, a, b, order in %s on line %d
