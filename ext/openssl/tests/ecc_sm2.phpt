--TEST--
openssl_*() with OPENSSL_KEYTYPE_EC for SM2
--EXTENSIONS--
openssl
--SKIPIF--
<?php
if (!defined("OPENSSL_KEYTYPE_EC")) die("skip EC disabled");
if (!in_array('SM2', openssl_get_curve_names())) die("skip SM2 disabled");
?>
--FILE--
<?php
// EC - generate SM2 keypair with curve_name
echo "Testing openssl_pkey_new with ec curve_name SM2\n";
$ec = openssl_pkey_new(array(
    'ec'=> array(
        'curve_name' => 'SM2',
    )
));

var_dump($ec);
$details = openssl_pkey_get_details($ec);
var_dump($details["bits"]);
var_dump(strlen($details["key"]));
var_dump($details["ec"]["curve_name"]);
var_dump($details["type"] == OPENSSL_KEYTYPE_EC);

// EC - generate SM2 keypair with curve_name
echo "Testing openssl_pkey_get_public from SM2 pem pubkey\n";
$public_key = openssl_pkey_get_public($details["key"]);
var_dump($public_key);
$details_public_key = openssl_pkey_get_details($public_key);
var_dump(strlen($details["key"]));
var_dump($details_public_key["ec"]["curve_name"]);
var_dump($details["type"] == OPENSSL_KEYTYPE_EC);
var_dump($details_public_key["ec"]["x"] === $details["ec"]["x"]);
var_dump($details_public_key["ec"]["y"] === $details["ec"]["y"]);

// EC - generate keypair with explicit parameters (SM2 curve)
echo "Testing openssl_pkey_new with ec explicit parameters (SM2 curve)\n";
$p = hex2bin('FFFFFFFEFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF00000000FFFFFFFFFFFFFFFF');
$a = hex2bin('FFFFFFFEFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF00000000FFFFFFFFFFFFFFFC');
$b = hex2bin('28E9FA9E9D9F5E344D5A9E4BCF6509A7F39789F515AB8F92DDBCBD414D940E93');
$g_x = hex2bin('32C4AE2C1F1981195F9904466A39C9948FE30BBFF2660BE1715A4589334C74C7');
$g_y = hex2bin('BC3736A2F4F6779C59BDCEE36B692153D0A9877CC62A474002DF32E52139F0A0');
$order = hex2bin('FFFFFFFEFFFFFFFFFFFFFFFFFFFFFFFF7203DF6B21C6052B53BBF40939D54123');

$ec = openssl_pkey_new(array(
    'ec'=> array(
        'p' => $p,
        'a' => $a,
        'b' => $b,
        'order' => $order,
        'g_x' => $g_x,
        'g_y' => $g_y
    )
));

$details = openssl_pkey_get_details($ec);
var_dump($details['bits']);
var_dump(strlen($details['key']));
var_dump($details['type'] == OPENSSL_KEYTYPE_EC);
$public_key = openssl_pkey_get_public($details["key"]);
$details_public_key = openssl_pkey_get_details($public_key);
var_dump(strlen($details["key"]));
var_dump($details["type"] == OPENSSL_KEYTYPE_EC);
var_dump($details_public_key["ec"]["x"] === $details["ec"]["x"]);
var_dump($details_public_key["ec"]["y"] === $details["ec"]["y"]);
?>
--EXPECTF--
Testing openssl_pkey_new with ec curve_name SM2
object(OpenSSLAsymmetricKey)#%d (0) {
}
int(256)
int(178)
string(3) "SM2"
bool(true)
Testing openssl_pkey_get_public from SM2 pem pubkey
object(OpenSSLAsymmetricKey)#%d (0) {
}
int(178)
string(3) "SM2"
bool(true)
bool(true)
bool(true)
Testing openssl_pkey_new with ec explicit parameters (SM2 curve)
int(256)
int(475)
bool(true)
int(475)
bool(true)
bool(true)
bool(true)
