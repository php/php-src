--TEST--
openssl_seal() tests
--SKIPIF--
<?php if (!extension_loaded("openssl")) print "skip"; ?>
--FILE--
<?php
// simple tests
$a = 1;
$b = array(1);
$c = array(1);
$d = array(1);

var_dump(openssl_seal($a, $b, $c, $d));

try {
    var_dump(openssl_seal($a, $a, $a, array()));
} catch (\ValueError $e) {
    echo $e->getMessage() . \PHP_EOL;
}

// tests with cert
$data = "openssl_open() test";
$pub_key = "file://" . __DIR__ . "/public.key";
$wrong = "wrong";

var_dump(openssl_seal($data, $sealed, $ekeys, array($pub_key)));                  // no output
var_dump(openssl_seal($data, $sealed, $ekeys, array($pub_key, $pub_key)));        // no output
var_dump(openssl_seal($data, $sealed, $ekeys, array($pub_key, $wrong)));

try {
    var_dump(openssl_seal($data, $sealed, $ekeys, array()));
} catch (\ValueError $e) {
    echo $e->getMessage() . \PHP_EOL;
}

var_dump(openssl_seal($data, $sealed, $ekeys, array($wrong)));

?>
--EXPECTF--
Warning: openssl_seal(): Not a public key (1th member of pubkeys) in %s on line %d
bool(false)
openssl_seal(): Argument #4 ($pubkeys) cannot be empty
int(19)
int(19)

Warning: openssl_seal(): Not a public key (2th member of pubkeys) in %s on line %d
bool(false)
openssl_seal(): Argument #4 ($pubkeys) cannot be empty

Warning: openssl_seal(): Not a public key (1th member of pubkeys) in %s on line %d
bool(false)
