--TEST--
Implicit float to int conversions should warn for variables
--FILE--
<?php

echo 'Bitwise ops:' . \PHP_EOL;

$float = 1.5;

$var = ~$float;
var_dump($var);

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
$modFloat = 6.5;
$var = $modFloat % 2;
var_dump($var);

$modFloat = 2.5;
$var = 9 % $modFloat;
var_dump($var);

echo 'Offset access:' . \PHP_EOL;
$offsetAccess = 2.5;
echo 'Arrays:' . \PHP_EOL;
// 2 warnings in total
$array = ['a', 'b', 'c'];
var_dump($array[$float]);
$array[$offsetAccess] = 'z';
var_dump($array);

echo 'Strings:' . \PHP_EOL;
// 2 warnings in total
$string = 'php';
var_dump($string[$float]);
$string[$offsetAccess] = 'z';
var_dump($string);

echo 'Function calls:' . \PHP_EOL;
function foo(int $a) {
    return $a;
}
var_dump(foo($float));

$cp = 60.5;
var_dump(chr($cp));

echo 'Function returns:' . \PHP_EOL;
function bar(): int {
    $var = 3.5;
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
--EXPECTF--
Bitwise ops:

Deprecated: Implicit conversion from float 1.5 to int loses precision in %s on line %d
int(-2)

Deprecated: Implicit conversion from float 1.5 to int loses precision in %s on line %d
int(3)

Deprecated: Implicit conversion from float 1.5 to int loses precision in %s on line %d
int(1)

Deprecated: Implicit conversion from float 1.5 to int loses precision in %s on line %d
int(2)

Deprecated: Implicit conversion from float 1.5 to int loses precision in %s on line %d
int(8)

Deprecated: Implicit conversion from float 1.5 to int loses precision in %s on line %d
int(0)

Deprecated: Implicit conversion from float 1.5 to int loses precision in %s on line %d
int(8)

Deprecated: Implicit conversion from float 1.5 to int loses precision in %s on line %d
int(0)

Deprecated: Implicit conversion from float 1.5 to int loses precision in %s on line %d
int(6)

Deprecated: Implicit conversion from float 1.5 to int loses precision in %s on line %d
int(1)
Modulo:

Deprecated: Implicit conversion from float 6.5 to int loses precision in %s on line %d
int(0)

Deprecated: Implicit conversion from float 2.5 to int loses precision in %s on line %d
int(1)
Offset access:
Arrays:

Deprecated: Implicit conversion from float 1.5 to int loses precision in %s on line %d
string(1) "b"

Deprecated: Implicit conversion from float 2.5 to int loses precision in %s on line %d
array(3) {
  [0]=>
  string(1) "a"
  [1]=>
  string(1) "b"
  [2]=>
  string(1) "z"
}
Strings:

Warning: String offset cast occurred in %s on line %d
string(1) "h"

Warning: String offset cast occurred in %s on line %d
string(3) "phz"
Function calls:

Deprecated: Implicit conversion from float 1.5 to int loses precision in %s on line %d
int(1)

Deprecated: Implicit conversion from float 60.5 to int loses precision in %s on line %d
string(1) "<"
Function returns:

Deprecated: Implicit conversion from float 3.5 to int loses precision in %s on line %d
int(3)
Typed property assignment:

Deprecated: Implicit conversion from float 1.5 to int loses precision in %s on line %d
int(1)
