--TEST--
gmp_strval() tests
--SKIPIF--
<?php if (!extension_loaded("gmp")) print "skip"; ?>
--FILE--
<?php

var_dump(gmp_strval(""));
try {
    var_dump(gmp_strval("", -1));
} catch (\ValueError $e) {
    echo $e->getMessage() . \PHP_EOL;
}

$fp = fopen(__FILE__, "r");
var_dump(gmp_strval($fp));

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

var_dump(gmp_strval(array(1,2)));
var_dump(gmp_strval(new stdclass));

echo "Done\n";
?>
--EXPECTF--
Warning: gmp_strval(): Unable to convert variable to GMP - string is not an integer in %s on line %d
bool(false)
gmp_strval(): Argument #2 ($base) must be between 2 and 62 or -2 and -36

Warning: gmp_strval(): Unable to convert variable to GMP - wrong type in %s on line %d
bool(false)
string(7) "9765456"
gmp_strval(): Argument #2 ($base) must be between 2 and 62 or -2 and -36
gmp_strval(): Argument #2 ($base) must be between 2 and 62 or -2 and -36
string(7) "9765456"
string(8) "-3373333"
gmp_strval(): Argument #2 ($base) must be between 2 and 62 or -2 and -36
gmp_strval(): Argument #2 ($base) must be between 2 and 62 or -2 and -36
string(8) "-3373333"

Warning: gmp_strval(): Unable to convert variable to GMP - wrong type in %s on line %d
bool(false)

Warning: gmp_strval(): Unable to convert variable to GMP - wrong type in %s on line %d
bool(false)
Done
