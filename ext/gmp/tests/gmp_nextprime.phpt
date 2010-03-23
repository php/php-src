--TEST--
gmp_nextprime()
--SKIPIF--
<?php if (!extension_loaded("gmp")) print "skip"; ?>
--FILE--
<?php

$n = gmp_nextprime(-1);
var_dump(gmp_strval($n));
$n = gmp_nextprime(0);
var_dump(gmp_strval($n));
$n = gmp_nextprime(-1000);
var_dump(gmp_strval($n));
$n = gmp_nextprime(1000);
var_dump(gmp_strval($n));
$n = gmp_nextprime(100000);
var_dump(gmp_strval($n));
$n = gmp_nextprime(array());
var_dump(gmp_strval($n));
$n = gmp_nextprime("");
var_dump(gmp_strval($n));
$n = gmp_nextprime(new stdclass());
var_dump(gmp_strval($n));
	
echo "Done\n";
?>
--EXPECTF--	
string(1) "2"
string(1) "2"
string(4) "-997"
string(4) "1009"
string(6) "100003"

Warning: gmp_nextprime(): Unable to convert variable to GMP - wrong type in %s on line %d
string(1) "0"
string(1) "0"

Warning: gmp_nextprime(): Unable to convert variable to GMP - wrong type in %s on line %d
string(1) "0"
Done
