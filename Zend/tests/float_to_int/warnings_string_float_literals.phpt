--TEST--
Implicit string float to int conversions should warn for literals
--FILE--
<?php

echo 'Bitwise ops:' . \PHP_EOL;
// 7 Warnings generated in total
$var = '1.5'|3;
var_dump($var);
$var = '1.5'&3;
var_dump($var);
$var = '1.5'^3;
var_dump($var);
$var = '1.5' << 3;
var_dump($var);
$var = '1.5' >> 3;
var_dump($var);
$var = 3 << '1.5';
var_dump($var);
$var = 3 >> '1.5';
var_dump($var);

echo 'Modulo:' . \PHP_EOL;
// 2 warnings in total
$var = '6.5' % 2;
var_dump($var);
$var = 9 % '2.5';
var_dump($var);

echo 'Function calls:' . \PHP_EOL;
function foo(int $a) {
    return $a;
}
var_dump(foo('1.5'));

var_dump(chr('60.5'));

echo 'Function returns:' . \PHP_EOL;
function bar(): int {
    return '3.5';
}
var_dump(bar());

echo 'Typed property assignment:' . \PHP_EOL;
class Test {
    public int $a;
}

$instance = new Test();
$instance->a = '1.5';
var_dump($instance->a);

?>
--EXPECTF--
Bitwise ops:

Deprecated: Implicit conversion from float-string "1.5" to int loses precision in %s on line %d
int(3)

Deprecated: Implicit conversion from float-string "1.5" to int loses precision in %s on line %d
int(1)

Deprecated: Implicit conversion from float-string "1.5" to int loses precision in %s on line %d
int(2)

Deprecated: Implicit conversion from float-string "1.5" to int loses precision in %s on line %d
int(8)

Deprecated: Implicit conversion from float-string "1.5" to int loses precision in %s on line %d
int(0)

Deprecated: Implicit conversion from float-string "1.5" to int loses precision in %s on line %d
int(6)

Deprecated: Implicit conversion from float-string "1.5" to int loses precision in %s on line %d
int(1)
Modulo:

Deprecated: Implicit conversion from float-string "6.5" to int loses precision in %s on line %d
int(0)

Deprecated: Implicit conversion from float-string "2.5" to int loses precision in %s on line %d
int(1)
Function calls:

Deprecated: Implicit conversion from float-string "1.5" to int loses precision in %s on line %d
int(1)

Deprecated: Implicit conversion from float-string "60.5" to int loses precision in %s on line %d
string(1) "<"
Function returns:

Deprecated: Implicit conversion from float-string "3.5" to int loses precision in %s on line %d
int(3)
Typed property assignment:

Deprecated: Implicit conversion from float-string "1.5" to int loses precision in %s on line %d
int(1)
