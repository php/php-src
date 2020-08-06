--TEST--
gmp_sub() tests
--SKIPIF--
<?php if (!extension_loaded("gmp")) print "skip"; ?>
--FILE--
<?php

try {
    var_dump(gmp_sub("", ""));
} catch (\TypeError $e) {
    echo $e->getMessage() . \PHP_EOL;
}
try {
    var_dump(gmp_sub(array(), array()));
} catch (\TypeError $e) {
    echo $e->getMessage() . \PHP_EOL;
}

var_dump($g = gmp_sub(10000, 10001));
var_dump(gmp_strval($g));
var_dump($g = gmp_sub(10000, -1));
var_dump(gmp_strval($g));

try {
    var_dump($g = gmp_sub(10000, new stdclass));
    var_dump(gmp_strval($g));
} catch (\TypeError $e) {
    echo $e->getMessage() . \PHP_EOL;
}
try {
    var_dump($g = gmp_sub(new stdclass, 100));
    var_dump(gmp_strval($g));
} catch (\TypeError $e) {
    echo $e->getMessage() . \PHP_EOL;
}

echo "Done\n";
?>
--EXPECT--
gmp_sub(): Argument #1 ($a) is not an integer string
gmp_sub(): Argument #1 ($a) must be of type GMP|string|int|bool, array given
object(GMP)#1 (1) {
  ["num"]=>
  string(2) "-1"
}
string(2) "-1"
object(GMP)#3 (1) {
  ["num"]=>
  string(5) "10001"
}
string(5) "10001"
gmp_sub(): Argument #2 ($b) must be of type GMP|string|int|bool, stdClass given
gmp_sub(): Argument #1 ($a) must be of type GMP|string|int|bool, stdClass given
Done
