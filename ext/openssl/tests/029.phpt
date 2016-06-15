--TEST--
openssl_pkey_new() with EC key
--SKIPIF--
<?php
if (!extension_loaded("openssl")) die("skip");
if (!defined("OPENSSL_KEYTYPE_EC")) die("skip no EC available");
?>
--FILE--
<?php
$key = openssl_pkey_get_private("file://" . dirname(__FILE__) . "/private_ec.key");

$details = openssl_pkey_get_details($key);
print_r($details);

echo "Use details an create the same key pair\n";
$key2 = openssl_pkey_new($details);
$details2 = openssl_pkey_get_details($key2);
print_r($details2);
var_dump(array_diff($details["ec"], $details2["ec"]));

echo "Missing 'd' parameter (private key part) => no private key informations\n";
$detailsCopy3 = $details;
unset($detailsCopy3["ec"]["d"]);
$key3 = openssl_pkey_new($detailsCopy3);
$details3 = openssl_pkey_get_details($key3);
print_r($details3);
var_dump(array_diff($details["ec"], $details3["ec"]));
$privateKey3 = openssl_pkey_get_private($key3);
var_dump($privateKey3);
$publicKey3 = openssl_pkey_get_public($key3);
var_dump($publicKey3);
var_dump($details["key"] === $details3["key"]);

echo "Missing 'x' parameter will not change the details. The public key is calculated from the private key 'd'\n";
$detailsCopy4 = $details;
unset($detailsCopy4["ec"]["x"]);
$key4 = openssl_pkey_new($detailsCopy4);
$details4 = openssl_pkey_get_details($key4);
print_r($details4);
var_dump(array_diff($details["ec"], $details4["ec"]));

echo "Missing 'y' parameter will not change the details. The public key is calculated from the private key 'd'\n";
$detailsCopy5 = $details;
unset($detailsCopy5["ec"]["y"]);
$key5 = openssl_pkey_new($detailsCopy5);
$details5 = openssl_pkey_get_details($key5);
print_r($details5);
var_dump(array_diff($details["ec"], $details5["ec"]));

echo "Missing 'd' and 'x' parameters will generate a new public key pair\n";
$detailsCopy6 = $details;
unset($detailsCopy6["ec"]["d"]);
unset($detailsCopy6["ec"]["x"]);
$key6 = openssl_pkey_new($detailsCopy6);
$details6 = openssl_pkey_get_details($key6);
print_r($details6);
var_dump(array_diff($details["ec"], $details6["ec"]));

echo "Missing 'd' and 'y' parameters will generate a new public key pair\n";
$detailsCopy7 = $details;
unset($detailsCopy7["ec"]["d"]);
unset($detailsCopy7["ec"]["y"]);
$key7 = openssl_pkey_new($detailsCopy7);
$details7 = openssl_pkey_get_details($key7);
print_r($details7);
var_dump(array_diff($details["ec"], $details7["ec"]));

// Tests vectors from http://point-at-infinity.org/ecc/nisttv
echo "Create a private key from scratch 1\n";
$detailsFromScratch8 = array(
	"ec" => array(
		"curve_name" => "prime256v1",
		"d" => "\1",
	),
);
$key8 = openssl_pkey_new($detailsFromScratch8);
$details8 = openssl_pkey_get_details($key8);
var_dump(strtoupper(bin2hex($details8["ec"]["x"])) === "6B17D1F2E12C4247F8BCE6E563A440F277037D812DEB33A0F4A13945D898C296");
var_dump(strtoupper(bin2hex($details8["ec"]["y"])) === "4FE342E2FE1A7F9B8EE7EB4A7C0F9E162BCE33576B315ECECBB6406837BF51F5");

echo "Create a private key from scratch 2\n";
$detailsFromScratch9 = array(
	"ec" => array(
		"curve_name" => "prime192v1",
		"d" => hex2bin("7FFFFFFFFE0000007FFFFE003FFFFFE0007FFF1FFFFE0800"),
	),
);
$key9 = openssl_pkey_new($detailsFromScratch9);
$details9 = openssl_pkey_get_details($key9);
var_dump(strtoupper(bin2hex($details9["ec"]["x"])) === "45DAF0A306121BDB3B82E734CB44FDF65C9930F0E4FD2068");
var_dump(strtoupper(bin2hex($details9["ec"]["y"])) === "F039FACE58EB7DE34E3374ADB28DF81F019C4548BAA75B64");
?>
--EXPECTF--
Array
(
    [bits] => 256
    [key] => -----BEGIN PUBLIC KEY-----%a
-----END PUBLIC KEY-----

    [ec] => Array
        (
            [curve_name] => prime256v1
            [curve_oid] => 1.2.840.10045.3.1.7
            [x] => %a
            [y] => %a
            [d] => %a
        )

    [type] => 3
)
Use details an create the same key pair
Array
(
    [bits] => 256
    [key] => -----BEGIN PUBLIC KEY-----%a
-----END PUBLIC KEY-----

    [ec] => Array
        (
            [curve_name] => prime256v1
            [curve_oid] => 1.2.840.10045.3.1.7
            [x] => %a
            [y] => %a
            [d] => %a
        )

    [type] => 3
)
array(0) {
}
Missing 'd' parameter (private key part) => no private key informations
Array
(
    [bits] => 256
    [key] => -----BEGIN PUBLIC KEY-----%a
-----END PUBLIC KEY-----

    [ec] => Array
        (
            [curve_name] => prime256v1
            [curve_oid] => 1.2.840.10045.3.1.7
            [x] => %a
            [y] => %a
        )

    [type] => 3
)
array(1) {
  ["d"]=>
  string(32) "%a"
}

Warning: openssl_pkey_get_private(): supplied key param is a public key in %s on line %d
bool(false)
resource(%d) of type (OpenSSL key)
bool(true)
Missing 'x' parameter will not change the details. The public key is calculated from the private key 'd'
Array
(
    [bits] => 256
    [key] => -----BEGIN PUBLIC KEY-----%a
-----END PUBLIC KEY-----

    [ec] => Array
        (
            [curve_name] => prime256v1
            [curve_oid] => 1.2.840.10045.3.1.7
            [x] => %a
            [y] => %a
            [d] => %a
        )

    [type] => 3
)
array(0) {
}
Missing 'y' parameter will not change the details. The public key is calculated from the private key 'd'
Array
(
    [bits] => 256
    [key] => -----BEGIN PUBLIC KEY-----%a
-----END PUBLIC KEY-----

    [ec] => Array
        (
            [curve_name] => prime256v1
            [curve_oid] => 1.2.840.10045.3.1.7
            [x] => %a
            [y] => %a
            [d] => %a
        )

    [type] => 3
)
array(0) {
}
Missing 'd' and 'x' parameters will generate a new public key pair
Array
(
    [bits] => 256
    [key] => -----BEGIN PUBLIC KEY-----%a
-----END PUBLIC KEY-----

    [ec] => Array
        (
            [curve_name] => prime256v1
            [curve_oid] => 1.2.840.10045.3.1.7
            [x] => %a
            [y] => %a
            [d] => %a
        )

    [type] => 3
)
array(3) {
  ["x"]=>
  string(32) "%a"
  ["y"]=>
  string(32) "%a"
  ["d"]=>
  string(32) "%a"
}
Missing 'd' and 'y' parameters will generate a new public key pair
Array
(
    [bits] => 256
    [key] => -----BEGIN PUBLIC KEY-----%a
-----END PUBLIC KEY-----

    [ec] => Array
        (
            [curve_name] => prime256v1
            [curve_oid] => 1.2.840.10045.3.1.7
            [x] => %a
            [y] => %a
            [d] => %a
        )

    [type] => 3
)
array(3) {
  ["x"]=>
  string(32) "%a"
  ["y"]=>
  string(32) "%a"
  ["d"]=>
  string(32) "%a"
}
Create a private key from scratch 1
bool(true)
bool(true)
Create a private key from scratch 2
bool(true)
bool(true)
