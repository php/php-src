--TEST--
gmp_invert() basic tests
--SKIPIF--
<?php if (!extension_loaded("gmp")) print "skip"; ?>
--FILE--
<?php

var_dump(gmp_strval(gmp_invert(123123,5467624)));
var_dump(gmp_strval(gmp_invert(123123,"3333334345467624")));
var_dump(gmp_strval(gmp_invert("12312323213123123",7624)));
var_dump(gmp_strval(gmp_invert(444,0)));
var_dump(gmp_strval(gmp_invert(0,28347)));
var_dump(gmp_strval(gmp_invert(-12,456456)));
var_dump(gmp_strval(gmp_invert(234234,-435345)));

$n = gmp_init("349827349623423452345");
$n1 = gmp_init("3498273496234234523451");

var_dump(gmp_strval(gmp_invert($n, $n1)));
var_dump(gmp_strval(gmp_invert($n1, $n)));

var_dump(gmp_invert($n1, $n, 10));
var_dump(gmp_invert($n1));
var_dump(gmp_invert(array(), 1));
var_dump(gmp_invert(1, array()));
var_dump(gmp_invert(array(), array()));

echo "Done\n";
?>
--EXPECTF--	
string(7) "2293131"
string(1) "0"
string(4) "5827"
string(1) "0"
string(1) "0"
string(1) "0"
string(1) "0"
string(22) "3498273496234234523441"
string(1) "1"

Warning: Wrong parameter count for gmp_invert() in %s on line %d
NULL

Warning: Wrong parameter count for gmp_invert() in %s on line %d
NULL

Warning: gmp_invert(): Unable to convert variable to GMP - wrong type in %s on line %d
bool(false)

Warning: gmp_invert(): Unable to convert variable to GMP - wrong type in %s on line %d
bool(false)

Warning: gmp_invert(): Unable to convert variable to GMP - wrong type in %s on line %d
bool(false)
Done
