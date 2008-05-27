--TEST--
gmp_prob_prime() basic tests
--SKIPIF--
<?php if (!extension_loaded("gmp")) print "skip"; ?>
--FILE--
<?php

var_dump(gmp_prob_prime(10));
var_dump(gmp_prob_prime("7"));
var_dump(gmp_prob_prime(17));
var_dump(gmp_prob_prime(-31));
var_dump(gmp_prob_prime("172368715471481723"));

var_dump(gmp_prob_prime(10));
var_dump(gmp_prob_prime("7"));
var_dump(gmp_prob_prime(17));
var_dump(gmp_prob_prime(-31));
var_dump(gmp_prob_prime("172368715471481723"));

for ($i = -1; $i < 12; $i++) {
	var_dump(gmp_prob_prime((773*$i)-($i*7)-1, $i));
	$n = gmp_init("23476812735411");
	var_dump(gmp_prob_prime(gmp_add($n, $i-1), $i));
}

$n = gmp_init("19481923");
var_dump(gmp_prob_prime($n));
$n = gmp_init(0);
var_dump(gmp_prob_prime($n));

var_dump(gmp_prob_prime());
var_dump(gmp_prob_prime(array()));

echo "Done\n";
?>
--EXPECTF--
int(0)
int(2)
int(2)
int(2)
int(0)
int(0)
int(2)
int(2)
int(2)
int(0)
int(0)
int(0)
int(0)
int(0)
int(0)
int(0)
int(2)
int(0)
int(2)
int(0)
int(0)
int(0)
int(0)
int(0)
int(0)
int(0)
int(0)
int(0)
int(0)
int(0)
int(0)
int(0)
int(0)
int(0)
int(0)
int(0)
int(0)
int(0)

Warning: gmp_prob_prime() expects at least 1 parameter, 0 given in %s on line %d
NULL

Warning: gmp_prob_prime(): Unable to convert variable to GMP - wrong type in %s on line %d
bool(false)
Done
