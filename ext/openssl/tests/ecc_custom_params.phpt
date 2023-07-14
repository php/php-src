--TEST--
openssl_*() with OPENSSL_KEYTYPE_EC for ec custom params
--EXTENSIONS--
openssl
--SKIPIF--
<?php
if (!defined("OPENSSL_KEYTYPE_EC")) die("skip EC disabled");
?>
--FILE--
<?php
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

// EC - generate keypair from priv_key "d" with explicit parameters (OSCCA WAPIP192v1 Elliptic curve)
echo "Testing openssl_pkey_new with ec explicit parameters\n";
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
// EC - invalid curve_name
$ec = openssl_pkey_new(array(
    'ec'=> array(
        'curve_name' => 'invalid_curve_name',
    )
));
var_dump($ec);

// EC - missing all params
$ec = openssl_pkey_new(array(
    'ec'=> array()
));
var_dump($ec);

// EC - missing "p" param
$ec = openssl_pkey_new(array(
    'ec'=> array(
        'a' => $a,
        'b' => $b,
        'order' => $order
    )
));
var_dump($ec);

// EC - missing "generator" or "g_x" and "g_y" param
$ec = openssl_pkey_new(array(
    'ec'=> array(
        'p' => $p,
        'a' => $a,
        'b' => $b,
        'order' => $order
    )
));
var_dump($ec);
?>
--EXPECTF--
Testing openssl_pkey_new with ec curve_name
object(OpenSSLAsymmetricKey)#%d (0) {
}
bool(true)
Testing openssl_pkey_new with ec explicit parameters
bool(true)
bool(true)
bool(true)
Testing openssl_pkey_new with ec missing params 

Warning: openssl_pkey_new(): Unknown elliptic curve (short) name invalid_curve_name in %s on line %d
bool(false)

Warning: openssl_pkey_new(): Missing params: curve_name in %s on line %d
bool(false)

Warning: openssl_pkey_new(): Missing params: curve_name or p, a, b, order in %s on line %d
bool(false)

Warning: openssl_pkey_new(): Missing params: generator or g_x and g_y in %s on line %d
bool(false)
