--TEST--
gmp_nextprime()
--SKIPIF--
<?php if (!extension_loaded("gmp")) print "skip";
?>
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
try {
    $n = gmp_nextprime(array());
    var_dump(gmp_strval($n));
} catch (\TypeError $e) {
    echo $e->getMessage() . \PHP_EOL;
}
try {
    $n = gmp_nextprime("");
    var_dump(gmp_strval($n));
} catch (\ValueError $e) {
    echo $e->getMessage() . \PHP_EOL;
}
try {
    $n = gmp_nextprime(new stdclass());
    var_dump(gmp_strval($n));
} catch (\TypeError $e) {
    echo $e->getMessage() . \PHP_EOL;
}

echo "Done\n";
?>
--EXPECT--
string(1) "2"
string(1) "2"
string(1) "2"
string(4) "1009"
string(6) "100003"
gmp_nextprime(): Argument #1 ($num) must be of type GMP|string|int, array given
gmp_nextprime(): Argument #1 ($num) is not an integer string
gmp_nextprime(): Argument #1 ($num) must be of type GMP|string|int, stdClass given
Done
