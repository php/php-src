--TEST--
openssl_seal() tests
--EXTENSIONS--
openssl
--FILE--
<?php
// simple tests
$a = 1;
$b = array(1);
$c = array(1);
$d = array(1);
$method = "AES-128-ECB";

var_dump(openssl_seal($a, $b, $c, $d, $method));

try {
    var_dump(openssl_seal($a, $a, $a, array(), $method));
} catch (\ValueError $e) {
    echo $e->getMessage() . \PHP_EOL;
}

// tests with cert
$data = "openssl_open() test";
$pub_key = "file://" . __DIR__ . "/public.key";
$wrong = "wrong";

var_dump(openssl_seal($data, $sealed, $ekeys, array($pub_key), $method));           // no output
var_dump(openssl_seal($data, $sealed, $ekeys, array($pub_key, $pub_key), $method)); // no output
var_dump(openssl_seal($data, $sealed, $ekeys, array($pub_key, $wrong), $method));

try {
    var_dump(openssl_seal($data, $sealed, $ekeys, array(), $method));
} catch (\ValueError $e) {
    echo $e->getMessage() . \PHP_EOL;
}

var_dump(openssl_seal($data, $sealed, $ekeys, array($wrong), $method));

?>
--EXPECTF--
Warning: openssl_seal(): Not a public key (1th member of pubkeys) in %s on line %d
bool(false)
openssl_seal(): Argument #4 ($public_key) cannot be empty
int(32)
int(32)

Warning: openssl_seal(): Not a public key (2th member of pubkeys) in %s on line %d
bool(false)
openssl_seal(): Argument #4 ($public_key) cannot be empty

Warning: openssl_seal(): Not a public key (1th member of pubkeys) in %s on line %d
bool(false)
