--TEST--
openssl: test key type and bit length enforcement in php_openssl_generate_private_key
--EXTENSIONS--
openssl
--SKIPIF--
<?php
if (!defined("OPENSSL_KEYTYPE_RSA")) die("skip RSA disabled");
if (!defined("OPENSSL_KEYTYPE_DSA")) die("skip DSA disabled");
if (!defined("OPENSSL_KEYTYPE_DH")) die("skip DH disabled");
if (!defined("OPENSSL_KEYTYPE_EC")) die("skip EC disabled");
?>
--FILE--
<?php
function test_key($type, $bits = null) {
    $args = [];
    switch ($type) {
        case OPENSSL_KEYTYPE_RSA:
            $args['private_key_type'] = OPENSSL_KEYTYPE_RSA;
            if ($bits !== null) $args['private_key_bits'] = $bits;
            break;
        case OPENSSL_KEYTYPE_DSA:
            $args['private_key_type'] = OPENSSL_KEYTYPE_DSA;
            if ($bits !== null) $args['private_key_bits'] = $bits;
            break;
        case OPENSSL_KEYTYPE_DH:
            $args['private_key_type'] = OPENSSL_KEYTYPE_DH;
            if ($bits !== null) $args['private_key_bits'] = $bits;
            break;
        case OPENSSL_KEYTYPE_EC:
            $args['curve_name'] = 'prime256v1';
            $args['private_key_type'] = OPENSSL_KEYTYPE_EC;
            break;
    }
    $key = @openssl_pkey_new($args);
    var_dump($key !== false);
}

// Should fail: RSA, DSA, DH with bits < MIN_KEY_LENGTH
foreach ([OPENSSL_KEYTYPE_RSA, OPENSSL_KEYTYPE_DSA, OPENSSL_KEYTYPE_DH] as $type) {
    test_key($type, 256); // too short
}
// Should succeed: RSA, DSA, DH with bits >= MIN_KEY_LENGTH
foreach ([OPENSSL_KEYTYPE_RSA, OPENSSL_KEYTYPE_DSA, OPENSSL_KEYTYPE_DH] as $type) {
    test_key($type, 1024); // valid, but small to keep test fast
}
// Should succeed: EC with curve only
 test_key(OPENSSL_KEYTYPE_EC);
// Should succeed: EC with bits too low
 test_key(OPENSSL_KEYTYPE_EC, 256);
?>
--EXPECT--
bool(false)
bool(false)
bool(false)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)