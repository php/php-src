--TEST--
String increment on a variety of strings
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
];

$strings = [
    // Empty string
    "",
    // String increments are unaware of being "negative"
    "-cc",
    // Trailing whitespace
    "Z ",
    // Leading whitespace
    " Z",
    // Non-ASCII characters
    "é",
    "あいうえお",
    "α",
    "ω",
    "Α",
    "Ω",
    // With period
    "foo1.txt",
    "1f.5",
    // With multiple period
    "foo.1.txt",
    "1.f.5",
];

foreach ($strictlyAlphaNumeric as $s) {
    var_dump(++$s);
}
foreach ($strings as $s) {
    var_dump(++$s);
}

// Will get converted to float on the second increment as it gets changed to a
// string interpretable as a number in scientific notation
$s = "5d9";
var_dump(++$s); // string(3) "5e0"
var_dump(++$s); // float(6)
?>
--EXPECTF--
string(2) "Ba"
string(2) "bA"
string(2) "B0"
string(2) "b0"
string(3) "AAa"
string(3) "aaA"
string(3) "10a"
string(3) "10A"

Deprecated: Increment on non-alphanumeric string is deprecated in %s on line %d
string(1) "1"

Deprecated: Increment on non-alphanumeric string is deprecated in %s on line %d
string(3) "-cd"

Deprecated: Increment on non-alphanumeric string is deprecated in %s on line %d
string(2) "Z "

Deprecated: Increment on non-alphanumeric string is deprecated in %s on line %d
string(2) " A"

Deprecated: Increment on non-alphanumeric string is deprecated in %s on line %d
string(2) "é"

Deprecated: Increment on non-alphanumeric string is deprecated in %s on line %d
string(15) "あいうえお"

Deprecated: Increment on non-alphanumeric string is deprecated in %s on line %d
string(2) "α"

Deprecated: Increment on non-alphanumeric string is deprecated in %s on line %d
string(2) "ω"

Deprecated: Increment on non-alphanumeric string is deprecated in %s on line %d
string(2) "Α"

Deprecated: Increment on non-alphanumeric string is deprecated in %s on line %d
string(2) "Ω"

Deprecated: Increment on non-alphanumeric string is deprecated in %s on line %d
string(8) "foo1.txu"

Deprecated: Increment on non-alphanumeric string is deprecated in %s on line %d
string(4) "1f.6"

Deprecated: Increment on non-alphanumeric string is deprecated in %s on line %d
string(9) "foo.1.txu"

Deprecated: Increment on non-alphanumeric string is deprecated in %s on line %d
string(5) "1.f.6"
string(3) "5e0"
float(6)
