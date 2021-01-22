--TEST--
gmp_strval() tests
--SKIPIF--
<?php if (!extension_loaded("gmp")) print "skip"; ?>
--FILE--
<?php

try {
    var_dump(gmp_strval(""));
} catch (\ValueError $e) {
    echo $e->getMessage() . \PHP_EOL;
}
try {
    var_dump(gmp_strval("", -1));
} catch (\ValueError $e) {
    echo $e->getMessage() . \PHP_EOL;
}

$fp = fopen(__FILE__, "r");
try {
    var_dump(gmp_strval($fp));
} catch (\TypeError $e) {
    echo $e->getMessage() . \PHP_EOL;
}

$g = gmp_init("9765456");
var_dump(gmp_strval($g));
try {
    var_dump(gmp_strval($g, -1));
} catch (\ValueError $e) {
    echo $e->getMessage() . \PHP_EOL;
}
try {
    var_dump(gmp_strval($g, 100000));
} catch (\ValueError $e) {
    echo $e->getMessage() . \PHP_EOL;
}
var_dump(gmp_strval($g, 10));

$g = gmp_init("-3373333");
var_dump(gmp_strval($g));
try {
    var_dump(gmp_strval($g, -1));
} catch (\ValueError $e) {
    echo $e->getMessage() . \PHP_EOL;
}
try {
    var_dump(gmp_strval($g, 100000));
} catch (\ValueError $e) {
    echo $e->getMessage() . \PHP_EOL;
}
var_dump(gmp_strval($g, 10));

try {
    var_dump(gmp_strval(array(1,2)));
} catch (\TypeError $e) {
    echo $e->getMessage() . \PHP_EOL;
}
try {
    var_dump(gmp_strval(new stdclass));
} catch (\TypeError $e) {
    echo $e->getMessage() . \PHP_EOL;
}

echo "Done\n";
?>
--EXPECT--
gmp_strval(): Argument #1 ($num) is not an integer string
gmp_strval(): Argument #2 ($base) must be between 2 and 62, or -2 and -36
gmp_strval(): Argument #1 ($num) must be of type GMP|string|int, resource given
string(7) "9765456"
gmp_strval(): Argument #2 ($base) must be between 2 and 62, or -2 and -36
gmp_strval(): Argument #2 ($base) must be between 2 and 62, or -2 and -36
string(7) "9765456"
string(8) "-3373333"
gmp_strval(): Argument #2 ($base) must be between 2 and 62, or -2 and -36
gmp_strval(): Argument #2 ($base) must be between 2 and 62, or -2 and -36
string(8) "-3373333"
gmp_strval(): Argument #1 ($num) must be of type GMP|string|int, array given
gmp_strval(): Argument #1 ($num) must be of type GMP|string|int, stdClass given
Done
