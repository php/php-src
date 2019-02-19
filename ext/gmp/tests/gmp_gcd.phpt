--TEST--
gmp_gcd() basic tests
--SKIPIF--
<?php if (!extension_loaded("gmp")) print "skip"; ?>
--FILE--
<?php

var_dump(gmp_strval(gmp_gcd(234,12387)));
var_dump(gmp_strval(gmp_gcd(0,12387)));
var_dump(gmp_strval(gmp_gcd(224,0)));
var_dump(gmp_strval(gmp_gcd(-1,0)));
var_dump(gmp_strval(gmp_gcd(-1,0)));
var_dump(gmp_strval(gmp_gcd("12371238123","32618723123")));
var_dump(gmp_strval(gmp_gcd("7623456735","12372341234")));

$n = gmp_init("8127346234");
var_dump(gmp_strval(gmp_gcd($n,"12372341234")));
$n = gmp_init("8127346234");
var_dump(gmp_strval(gmp_gcd("7623456735",$n)));
$n = gmp_init("8127346234");
var_dump(gmp_strval(gmp_gcd($n,$n)));
$n = gmp_init("8127346234");
var_dump(gmp_strval(gmp_gcd($n,0)));

echo "Done\n";
?>
--EXPECT--
string(1) "3"
string(5) "12387"
string(3) "224"
string(1) "1"
string(1) "1"
string(1) "1"
string(1) "1"
string(1) "2"
string(1) "1"
string(10) "8127346234"
string(10) "8127346234"
Done
