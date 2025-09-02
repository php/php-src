--TEST--
str_decrement(): Decrementing various strings
--FILE--
<?php

$strictlyAlphaNumeric = [
    "Az",
    "aZ",
    "A9",
    "a9",
    // Carrying values until the beginning of the string (no underflow)
    "Za",
    "zA",
    "Z0",
    "z0",
    // Underflow, removing leading character
    "Aa",
    "aA",
    "A0",
    "a0",
    "10",
    "1A",
    "1a",
    "10a",
    // string interpretable as a number in scientific notation
    "5e6",
    // Interned strings
    "d",
    "D",
    "4",
    "1",
];

foreach ($strictlyAlphaNumeric as $s) {
    var_dump(str_decrement($s));
    var_dump($s);
}

?>
--EXPECT--
string(2) "Ay"
string(2) "Az"
string(2) "aY"
string(2) "aZ"
string(2) "A8"
string(2) "A9"
string(2) "a8"
string(2) "a9"
string(2) "Yz"
string(2) "Za"
string(2) "yZ"
string(2) "zA"
string(2) "Y9"
string(2) "Z0"
string(2) "y9"
string(2) "z0"
string(1) "z"
string(2) "Aa"
string(1) "Z"
string(2) "aA"
string(1) "9"
string(2) "A0"
string(1) "9"
string(2) "a0"
string(1) "9"
string(2) "10"
string(1) "Z"
string(2) "1A"
string(1) "z"
string(2) "1a"
string(2) "9z"
string(3) "10a"
string(3) "5e5"
string(3) "5e6"
string(1) "c"
string(1) "d"
string(1) "C"
string(1) "D"
string(1) "3"
string(1) "4"
string(1) "0"
string(1) "1"
