--TEST--
openssl_seal() tests
--SKIPIF--
<?php if (!extension_loaded("openssl")) print "skip"; ?>
--FILE--
<?php

$a = 1;
$b = array(1);
$c = array(1);
$d = array(1);

var_dump(openssl_seal($a, $b, $c, $d));
var_dump(openssl_seal($a, $a, $a, array()));
var_dump(openssl_seal($c, $c, $c, 1));
var_dump(openssl_seal($b, $b, $b, ""));

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
Done
