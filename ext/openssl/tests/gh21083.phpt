--TEST--
GH-21083 (openssl_pkey_new() fails for EC keys when private_key_bits is not set)
--EXTENSIONS--
openssl
--SKIPIF--
<?php if (!defined("OPENSSL_KEYTYPE_EC")) die("skip EC disabled"); ?>
--ENV--
OPENSSL_CONF=
--FILE--
<?php
// Create a minimal openssl.cnf without default_bits (simulates OpenSSL 3.6 default config)
$conf = tempnam(sys_get_temp_dir(), 'ossl');
file_put_contents($conf, "[req]\ndistinguished_name = req_dn\n[req_dn]\n");

// EC key - size is determined by the curve, private_key_bits should not be required
$key = openssl_pkey_new([
    'config' => $conf,
    'private_key_type' => OPENSSL_KEYTYPE_EC,
    'curve_name' => 'prime256v1',
]);
var_dump($key !== false);
$details = openssl_pkey_get_details($key);
var_dump($details['bits']);
var_dump($details['type'] === OPENSSL_KEYTYPE_EC);
echo "EC OK\n";

// X25519 - fixed size key, private_key_bits should not be required
if (defined('OPENSSL_KEYTYPE_X25519')) {
    $key = openssl_pkey_new([
        'config' => $conf,
        'private_key_type' => OPENSSL_KEYTYPE_X25519,
    ]);
    var_dump($key !== false);
    echo "X25519 OK\n";
} else {
    echo "bool(true)\nX25519 OK\n";
}

// Ed25519 - fixed size key, private_key_bits should not be required
if (defined('OPENSSL_KEYTYPE_ED25519')) {
    $key = openssl_pkey_new([
        'config' => $conf,
        'private_key_type' => OPENSSL_KEYTYPE_ED25519,
    ]);
    var_dump($key !== false);
    echo "Ed25519 OK\n";
} else {
    echo "bool(true)\nEd25519 OK\n";
}

unlink($conf);
?>
--EXPECT--
bool(true)
int(256)
bool(true)
EC OK
bool(true)
X25519 OK
bool(true)
Ed25519 OK
