--TEST--
str_increment(): Incrementing various strings
--FILE--
<?php

$strictlyAlphaNumeric = [
    "Az",
    "aZ",
    "A9",
    "a9",
    // Carrying values until the beginning of the string
    "Zz",
    "zZ",
    "9z",
    "9Z",
    // string interpretable as a number in scientific notation
    "5e6",
    // Interned strings
    "d",
    "D",
    "4",
];

foreach ($strictlyAlphaNumeric as $s) {
    var_dump(str_increment($s));
    var_dump($s);
}

?>
--EXPECT--
string(2) "Ba"
string(2) "Az"
string(2) "bA"
string(2) "aZ"
string(2) "B0"
string(2) "A9"
string(2) "b0"
string(2) "a9"
string(3) "AAa"
string(2) "Zz"
string(3) "aaA"
string(2) "zZ"
string(3) "10a"
string(2) "9z"
string(3) "10A"
string(2) "9Z"
string(3) "5e7"
string(3) "5e6"
string(1) "e"
string(1) "d"
string(1) "E"
string(1) "D"
string(1) "5"
string(1) "4"
