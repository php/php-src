--TEST--
using different variables to access string offsets
--FILE--
<?php

$str = "Sitting on a corner all alone, staring from the bottom of his soul";

var_dump($str[1]);
var_dump($str[0.0836]);
var_dump($str[NULL]);
try {
    var_dump($str["run away"]);
} catch (\TypeError $e) {
    echo $e->getMessage() . \PHP_EOL;
}
var_dump($str["13"]);
try {
    var_dump($str["14.5"]);
} catch (\TypeError $e) {
    echo $e->getMessage() . \PHP_EOL;
}
var_dump($str["15 and then some"]);

var_dump($str[TRUE]);
var_dump($str[FALSE]);

$fp = fopen(__FILE__, "r");
try {
    var_dump($str[$fp]);
} catch (Error $e) {
    echo $e->getMessage(), "\n";
}

$obj = new stdClass;
try {
    var_dump($str[$obj]);
} catch (Error $e) {
    echo $e->getMessage(), "\n";
}

$arr = Array(1,2,3);
try {
    var_dump($str[$arr]);
} catch (Error $e) {
    echo $e->getMessage(), "\n";
}

echo "Done\n";
?>
--EXPECTF--
string(1) "i"

Warning: String offset cast occurred in %s on line %d
string(1) "S"

Warning: String offset cast occurred in %s on line %d
string(1) "S"
Cannot access offset of type string on string
string(1) "c"
Cannot access offset of type string on string

Warning: Illegal string offset "15 and then some" in %s on line %d
string(1) "r"

Warning: String offset cast occurred in %s on line %d
string(1) "i"

Warning: String offset cast occurred in %s on line %d
string(1) "S"
Cannot access offset of type resource on string
Cannot access offset of type stdClass on string
Cannot access offset of type array on string
Done
