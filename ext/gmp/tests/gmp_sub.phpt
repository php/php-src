--TEST--
gmp_sub() tests
--EXTENSIONS--
gmp
--FILE--
<?php

try {
    var_dump(gmp_sub("", ""));
} catch (\ValueError $e) {
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
gmp_sub(): Argument #1 ($num1) is not an integer string
gmp_sub(): Argument #1 ($num1) must be of type GMP|string|int, array given
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
gmp_sub(): Argument #2 ($num2) must be of type GMP|string|int, stdClass given
gmp_sub(): Argument #1 ($num1) must be of type GMP|string|int, stdClass given
Done
