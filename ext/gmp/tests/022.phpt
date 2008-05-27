--TEST--
gmp_gcdext() basic tests
--SKIPIF--
<?php if (!extension_loaded("gmp")) print "skip"; ?>
--FILE--
<?php

$n = gmp_init("34293864345");
$n1 = gmp_init("23434293864345");

$a = array(
	array(123,45),
	array(4341,9734),
	array(23487,333),
	array(-234234,-123123),
	array(-100,-2234),
	array(345,"34587345"),
	array(345,"0"),
	array("345556456",345873),
	array("34545345556456","323432445873"),
	array($n, $n1),
	);

foreach ($a as $val) {
	$r = gmp_gcdext($val[0],$val[1]);
	var_dump(gmp_strval($r['g']));
	var_dump(gmp_strval($r['s']));
	var_dump(gmp_strval($r['t']));
}

var_dump(gmp_gcdext($val[0],array()));
var_dump(gmp_gcdext(array(),array()));
var_dump(gmp_gcdext(array(),array(),1));
var_dump(gmp_gcdext(array()));
var_dump(gmp_gcdext());

echo "Done\n";
?>
--EXPECTF--
unicode(1) "3"
unicode(2) "-4"
unicode(2) "11"
unicode(1) "1"
unicode(4) "-805"
unicode(3) "359"
unicode(1) "3"
unicode(2) "32"
unicode(5) "-2257"
unicode(4) "3003"
unicode(3) "-10"
unicode(2) "19"
unicode(1) "2"
unicode(2) "67"
unicode(2) "-3"
unicode(2) "15"
unicode(7) "-601519"
unicode(1) "6"
unicode(3) "345"
unicode(1) "1"
unicode(1) "0"
unicode(1) "1"
unicode(5) "84319"
unicode(9) "-84241831"
unicode(1) "1"
unicode(13) "-156252240050"
unicode(14) "16689072773537"
unicode(3) "195"
unicode(11) "46994884483"
unicode(9) "-68772552"

Warning: gmp_gcdext(): Unable to convert variable to GMP - wrong type in %s on line %d
bool(false)

Warning: gmp_gcdext(): Unable to convert variable to GMP - wrong type in %s on line %d
bool(false)

Warning: gmp_gcdext() expects exactly 2 parameters, 3 given in %s on line %d
NULL

Warning: gmp_gcdext() expects exactly 2 parameters, 1 given in %s on line %d
NULL

Warning: gmp_gcdext() expects exactly 2 parameters, 0 given in %s on line %d
NULL
Done
