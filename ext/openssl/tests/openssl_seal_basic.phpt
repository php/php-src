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
var_dump(openssl_seal($a, $a, $a, array()));
var_dump(openssl_seal($c, $c, $c, 1));
var_dump(openssl_seal($b, $b, $b, ""));

// tests with cert
$data = "openssl_open() test";
$pub_key = "file://" . dirname(__FILE__) . "/public.key";
$wrong = "wrong";

var_dump(openssl_seal($data, $sealed, $ekeys, array($pub_key)));                  // no output
var_dump(openssl_seal($data, $sealed, $ekeys, array($pub_key, $pub_key)));        // no output
var_dump(openssl_seal($data, $sealed, $ekeys, array($pub_key, $wrong)));
var_dump(openssl_seal($data, $sealed, $ekeys, $pub_key));
var_dump(openssl_seal($data, $sealed, $ekeys, array()));
var_dump(openssl_seal($data, $sealed, $ekeys, array($wrong)));

echo "Done\n";
?>
--EXPECTF--	
Warning: openssl_seal(): not a public key (1th member of pubkeys) in %s on line %d
bool(false)

Warning: openssl_seal(): Fourth argument to openssl_seal() must be a non-empty array in %s on line %d
bool(false)

Warning: openssl_seal() expects parameter 1 to be string, array given in %s on line %d
NULL

Warning: openssl_seal() expects parameter 1 to be string, array given in %s on line %d
NULL
int(19)
int(19)

Warning: openssl_seal(): not a public key (2th member of pubkeys) in %s on line %d
bool(false)

Warning: openssl_seal() expects parameter 4 to be array, string given in %s on line %d
NULL

Warning: openssl_seal(): Fourth argument to openssl_seal() must be a non-empty array in %s on line %d
bool(false)

Warning: openssl_seal(): not a public key (1th member of pubkeys) in %s on line %d
bool(false)
Done
