--TEST--
Implicit float to int conversions when float too large should warn, string offset variant, 32bit variant
--SKIPIF--
<?php
if (PHP_INT_SIZE != 4) die("skip this test is for 32bit platform only");
?>
--FILE--
<?php

$float = 10e120;
$string_float = (string) $float;

var_dump((int) $float);
var_dump((int) $string_float);

$string = 'Here is some text for good measure';

echo 'Attempt to read', \PHP_EOL;
try {
    echo 'Float', \PHP_EOL;
    var_dump($string[10e120]);
} catch (\TypeError) {
    echo 'TypeError', \PHP_EOL;
}
try {
    echo 'Float variable', \PHP_EOL;
    var_dump($string[$float]);
} catch (\TypeError) {
    echo 'TypeError', \PHP_EOL;
}
try {
    echo 'Float casted to string compile', \PHP_EOL;
    var_dump($string[(string) 10e120]);
} catch (\TypeError) {
    echo 'TypeError', \PHP_EOL;
}
try {
    echo 'Float string variable', \PHP_EOL;
    var_dump($string[$string_float]);
} catch (\TypeError) {
    echo 'TypeError', \PHP_EOL;
}

echo 'Attempt to assign', \PHP_EOL;
try {
    echo 'Float', \PHP_EOL;
    $string[10e120] = 'E';
    var_dump($string);
    $string = 'Here is some text for good measure';
} catch (\TypeError) {
    echo 'TypeError', \PHP_EOL;
}
try {
    echo 'Float variable', \PHP_EOL;
    $string[$float] = 'E';
    var_dump($string);
    $string = 'Here is some text for good measure';
} catch (\TypeError) {
    echo 'TypeError', \PHP_EOL;
}

try {
    $string[(string) 10e120] = 'E';
    var_dump($string);
} catch (\TypeError) {
    echo 'TypeError', \PHP_EOL;
}
var_dump($string);
try {
    $string[$string_float] = 'E';
} catch (\TypeError) {
    echo 'TypeError', \PHP_EOL;
}
var_dump($string);

?>
--EXPECTF--
int(0)
int(2147483647)
Attempt to read
Float

Warning: String offset cast occurred in %s on line %d
string(1) "H"
Float variable

Warning: String offset cast occurred in %s on line %d
string(1) "H"
Float casted to string compile
TypeError
Float string variable
TypeError
Attempt to assign
Float

Warning: String offset cast occurred in %s on line %d
string(34) "Eere is some text for good measure"
Float variable

Warning: String offset cast occurred in %s on line %d
string(34) "Eere is some text for good measure"
TypeError
string(34) "Here is some text for good measure"
TypeError
string(34) "Here is some text for good measure"
