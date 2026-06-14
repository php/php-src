--TEST--
Implicit string float to int conversions should not warn for variables if float has a fractional part equal to 0
--FILE--
<?php

echo 'Bitwise ops:' . \PHP_EOL;

$float = '1.0';

$var = $float|3;
var_dump($var);

$var = $float&3;
var_dump($var);

$var = $float^3;
var_dump($var);

$var = $float << 3;
var_dump($var);

$var = $float >> 3;
var_dump($var);

$var = $float;
$var <<= 3;
var_dump($var);

$var = $float;
$var >>= 3;
var_dump($var);

$var = 3 << $float;
var_dump($var);

$var = 3 >> $float;
var_dump($var);

echo 'Modulo:' . \PHP_EOL;
$modFloat = '6.0';
$var = $modFloat % 2;
var_dump($var);

$modFloat = '2.0';
$var = 9 % $modFloat;
var_dump($var);

echo 'Function calls:' . \PHP_EOL;
function foo(int $a) {
    return $a;
}
var_dump(foo($float));

$cp = '60.0';
var_dump(chr($cp));

echo 'Function returns:' . \PHP_EOL;
function bar(): int {
    $var = '3.0';
    return $var;
}
var_dump(bar());

echo 'Typed property assignment:' . \PHP_EOL;
class Test {
    public int $a;
}

$instance = new Test();
$instance->a = $float;
var_dump($instance->a);

?>
--EXPECT--
Bitwise ops:
int(3)
int(1)
int(2)
int(8)
int(0)
int(8)
int(0)
int(6)
int(1)
Modulo:
int(0)
int(1)
Function calls:
int(1)
string(1) "<"
Function returns:
int(3)
Typed property assignment:
int(1)
