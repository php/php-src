--TEST--
Case-insensitive string comparisons use case folding
--SKIPIF--
<?php require 'skipif.inc'; ?>
--FILE--
<?php

$tests = [
    ["K", "K"],
    ["k", "K"],
    ["Å", "Å"],
    ["å", "Å"],
    ["ß", "ẞ"],
    ["Θ", "ϴ"],
    ["θ", "ϴ"],
    ["ϑ", "ϴ"],
    ["Ω", "Ω"],
    ["ω", "Ω"],
    ["I", "ı"],
    ["i", "ı"],
];

foreach ($tests as list($a, $b)) {
    var_dump(mb_stripos($a, $b));
}

?>
--EXPECT--
int(0)
int(0)
int(0)
int(0)
int(0)
int(0)
int(0)
int(0)
int(0)
int(0)
bool(false)
bool(false)
