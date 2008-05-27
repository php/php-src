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
%s(1) "2"
%s(1) "2"
%s(4) "-997"
%s(4) "1009"
%s(6) "100003"

Warning: gmp_nextprime(): Unable to convert variable to GMP - wrong type in %s on line %d
%s(1) "0"
%s(1) "0"

Warning: gmp_nextprime(): Unable to convert variable to GMP - wrong type in %s on line %d
%s(1) "0"
Done
