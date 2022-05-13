--TEST--
GMP operator overloading
--EXTENSIONS--
gmp
--FILE--
<?php

$a = gmp_init(42);
$b = gmp_init(17);

var_dump($a + $b);
var_dump($a + 17);
var_dump(42 + $b);

var_dump($a - $b);
var_dump($a - 17);
var_dump(42 - $b);

var_dump($a * $b);
var_dump($a * 17);
var_dump(42 * $b);

var_dump($a / $b);
var_dump($a / 17);
var_dump(42 / $b);
try {
    var_dump($a / 0);
} catch (\DivisionByZeroError $e) {
    echo $e->getMessage() . \PHP_EOL;
}

var_dump($a % $b);
var_dump($a % 17);
var_dump(42 % $b);
try {
    var_dump($a % 0);
} catch (\DivisionByZeroError $e) {
    echo $e->getMessage() . \PHP_EOL;
}

var_dump($a ** $b);
var_dump($a ** 17);
var_dump(42 ** $b);

var_dump($a | $b);
var_dump($a | 17);
var_dump(42 | $b);

var_dump($a & $b);
var_dump($a & 17);
var_dump(42 & $b);

var_dump($a ^ $b);
var_dump($a ^ 17);
var_dump(42 ^ $b);

var_dump($a << $b);
var_dump($a << 17);
var_dump(42 << $b);

var_dump($a >> 2);
var_dump(-$a >> 2);

try {
    $a << -1;
} catch (ValueError $exception) {
    echo $exception->getMessage() . "\n";
}

try {
    $a >> -1;
} catch (ValueError $exception) {
    echo $exception->getMessage() . "\n";
}

var_dump(~$a);
var_dump(-$a);
var_dump(+$a);

var_dump($a == $b);
var_dump($a != $b);
var_dump($a < $b);
var_dump($a <= $b);
var_dump($a > $b);
var_dump($a >= $b);

var_dump($a == $a);
var_dump($a != $a);

var_dump($a == 42);
var_dump($a != 42);
var_dump($a < 42);
var_dump($a <= 42);
var_dump($a > 42);
var_dump($a >= 42);

try {
    var_dump($a == new stdClass);
} catch (\TypeError $e) {
    echo $e->getMessage() . \PHP_EOL;
}

$a += 1;
var_dump($a);
$a -= 1;
var_dump($a);

var_dump(++$a);
var_dump($a++);
var_dump($a);

var_dump(--$a);
var_dump($a--);
var_dump($a);

// Test operator that was not overloaded

var_dump($a . $b);
var_dump($a . '17');
var_dump('42' . $b);

$a .= '17';
var_dump($a);

?>
--EXPECT--
object(GMP)#3 (1) {
  ["num"]=>
  string(2) "59"
}
object(GMP)#3 (1) {
  ["num"]=>
  string(2) "59"
}
object(GMP)#3 (1) {
  ["num"]=>
  string(2) "59"
}
object(GMP)#3 (1) {
  ["num"]=>
  string(2) "25"
}
object(GMP)#3 (1) {
  ["num"]=>
  string(2) "25"
}
object(GMP)#3 (1) {
  ["num"]=>
  string(2) "25"
}
object(GMP)#3 (1) {
  ["num"]=>
  string(3) "714"
}
object(GMP)#3 (1) {
  ["num"]=>
  string(3) "714"
}
object(GMP)#3 (1) {
  ["num"]=>
  string(3) "714"
}
object(GMP)#3 (1) {
  ["num"]=>
  string(1) "2"
}
object(GMP)#3 (1) {
  ["num"]=>
  string(1) "2"
}
object(GMP)#3 (1) {
  ["num"]=>
  string(1) "2"
}
Division by zero
object(GMP)#4 (1) {
  ["num"]=>
  string(1) "8"
}
object(GMP)#4 (1) {
  ["num"]=>
  string(1) "8"
}
object(GMP)#4 (1) {
  ["num"]=>
  string(1) "8"
}
Modulo by zero
object(GMP)#3 (1) {
  ["num"]=>
  string(28) "3937657486715347520027492352"
}
object(GMP)#3 (1) {
  ["num"]=>
  string(28) "3937657486715347520027492352"
}
object(GMP)#3 (1) {
  ["num"]=>
  string(28) "3937657486715347520027492352"
}
object(GMP)#3 (1) {
  ["num"]=>
  string(2) "59"
}
object(GMP)#3 (1) {
  ["num"]=>
  string(2) "59"
}
object(GMP)#3 (1) {
  ["num"]=>
  string(2) "59"
}
object(GMP)#3 (1) {
  ["num"]=>
  string(1) "0"
}
object(GMP)#3 (1) {
  ["num"]=>
  string(1) "0"
}
object(GMP)#3 (1) {
  ["num"]=>
  string(1) "0"
}
object(GMP)#3 (1) {
  ["num"]=>
  string(2) "59"
}
object(GMP)#3 (1) {
  ["num"]=>
  string(2) "59"
}
object(GMP)#3 (1) {
  ["num"]=>
  string(2) "59"
}
object(GMP)#3 (1) {
  ["num"]=>
  string(7) "5505024"
}
object(GMP)#3 (1) {
  ["num"]=>
  string(7) "5505024"
}
object(GMP)#3 (1) {
  ["num"]=>
  string(7) "5505024"
}
object(GMP)#3 (1) {
  ["num"]=>
  string(2) "10"
}
object(GMP)#5 (1) {
  ["num"]=>
  string(3) "-11"
}
Shift must be greater than or equal to 0
Shift must be greater than or equal to 0
object(GMP)#5 (1) {
  ["num"]=>
  string(3) "-43"
}
object(GMP)#5 (1) {
  ["num"]=>
  string(3) "-42"
}
object(GMP)#5 (1) {
  ["num"]=>
  string(2) "42"
}
bool(false)
bool(true)
bool(false)
bool(false)
bool(true)
bool(true)
bool(true)
bool(false)
bool(true)
bool(false)
bool(false)
bool(true)
bool(false)
bool(true)
Number must be of type GMP|string|int, stdClass given
object(GMP)#4 (1) {
  ["num"]=>
  string(2) "43"
}
object(GMP)#1 (1) {
  ["num"]=>
  string(2) "42"
}
object(GMP)#4 (1) {
  ["num"]=>
  string(2) "43"
}
object(GMP)#4 (1) {
  ["num"]=>
  string(2) "43"
}
object(GMP)#1 (1) {
  ["num"]=>
  string(2) "44"
}
object(GMP)#4 (1) {
  ["num"]=>
  string(2) "43"
}
object(GMP)#4 (1) {
  ["num"]=>
  string(2) "43"
}
object(GMP)#1 (1) {
  ["num"]=>
  string(2) "42"
}
string(4) "4217"
string(4) "4217"
string(4) "4217"
string(4) "4217"
