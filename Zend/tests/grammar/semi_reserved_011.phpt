--TEST--
Edge case: casting operators and arglist
--FILE--
<?php

$test = function($const){ var_dump($const); };

const int = '<int>';
const integer = '<integer>';
const bool = '<bool>';
const boolean = '<boolean>';
const real = '<real>';
const double = '<double>';
const float = '<float>';
const string = '<string>';
const binary = '<binary>';
const object = '<object>';

$test(int);
$test(integer);
$test(bool);
$test(boolean);
$test(real);
$test(double);
$test(float);
$test(string);
$test(binary);
$test(object);

echo PHP_EOL, 'Done', PHP_EOL;
?>
--EXPECTF--
string(5) "<int>"
string(9) "<integer>"
string(6) "<bool>"
string(9) "<boolean>"
string(6) "<real>"
string(8) "<double>"
string(7) "<float>"
string(8) "<string>"
string(8) "<binary>"
string(8) "<object>"

Done
