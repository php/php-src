--TEST--
GMP operator overloading
--SKIPIF--
<?php if (!extension_loaded("gmp")) print "skip"; ?>
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
var_dump($a / 0);

var_dump($a % $b);
var_dump($a % 17);
var_dump(42 % $b);
var_dump($a % 0);

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

var_dump($a << -1);
var_dump($a >> -1);

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

var_dump($a == new stdClass);

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
--EXPECTF--
object(GMP)#%d (1) {
  ["num"]=>
  string(2) "59"
}
object(GMP)#%d (1) {
  ["num"]=>
  string(2) "59"
}
object(GMP)#%d (1) {
  ["num"]=>
  string(2) "59"
}
object(GMP)#%d (1) {
  ["num"]=>
  string(2) "25"
}
object(GMP)#%d (1) {
  ["num"]=>
  string(2) "25"
}
object(GMP)#%d (1) {
  ["num"]=>
  string(2) "25"
}
object(GMP)#%d (1) {
  ["num"]=>
  string(3) "714"
}
object(GMP)#%d (1) {
  ["num"]=>
  string(3) "714"
}
object(GMP)#%d (1) {
  ["num"]=>
  string(3) "714"
}
object(GMP)#%d (1) {
  ["num"]=>
  string(1) "2"
}
object(GMP)#%d (1) {
  ["num"]=>
  string(1) "2"
}
object(GMP)#%d (1) {
  ["num"]=>
  string(1) "2"
}

Warning: main(): Zero operand not allowed in %s on line %d
bool(false)
object(GMP)#%d (1) {
  ["num"]=>
  string(1) "8"
}
object(GMP)#%d (1) {
  ["num"]=>
  string(1) "8"
}
object(GMP)#%d (1) {
  ["num"]=>
  string(1) "8"
}

Warning: main(): Zero operand not allowed in %s on line %d
bool(false)
object(GMP)#%d (1) {
  ["num"]=>
  string(28) "3937657486715347520027492352"
}
object(GMP)#%d (1) {
  ["num"]=>
  string(28) "3937657486715347520027492352"
}
object(GMP)#%d (1) {
  ["num"]=>
  string(28) "3937657486715347520027492352"
}
object(GMP)#%d (1) {
  ["num"]=>
  string(2) "59"
}
object(GMP)#%d (1) {
  ["num"]=>
  string(2) "59"
}
object(GMP)#%d (1) {
  ["num"]=>
  string(2) "59"
}
object(GMP)#%d (1) {
  ["num"]=>
  string(1) "0"
}
object(GMP)#%d (1) {
  ["num"]=>
  string(1) "0"
}
object(GMP)#%d (1) {
  ["num"]=>
  string(1) "0"
}
object(GMP)#%d (1) {
  ["num"]=>
  string(2) "59"
}
object(GMP)#%d (1) {
  ["num"]=>
  string(2) "59"
}
object(GMP)#%d (1) {
  ["num"]=>
  string(2) "59"
}
object(GMP)#%d (1) {
  ["num"]=>
  string(7) "5505024"
}
object(GMP)#%d (1) {
  ["num"]=>
  string(7) "5505024"
}
object(GMP)#%d (1) {
  ["num"]=>
  string(7) "5505024"
}
object(GMP)#%d (1) {
  ["num"]=>
  string(2) "10"
}
object(GMP)#%d (1) {
  ["num"]=>
  string(3) "-11"
}

Warning: main(): Shift cannot be negative in %s on line %d
bool(false)

Warning: main(): Shift cannot be negative in %s on line %d
bool(false)
object(GMP)#%d (1) {
  ["num"]=>
  string(3) "-43"
}
object(GMP)#%d (1) {
  ["num"]=>
  string(3) "-42"
}
object(GMP)#%d (1) {
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

Warning: main(): Unable to convert variable to GMP - wrong type in %s on line %d
bool(false)
object(GMP)#%d (1) {
  ["num"]=>
  string(2) "43"
}
object(GMP)#%d (1) {
  ["num"]=>
  string(2) "42"
}
object(GMP)#%d (1) {
  ["num"]=>
  string(2) "43"
}
object(GMP)#%d (1) {
  ["num"]=>
  string(2) "43"
}
object(GMP)#%d (1) {
  ["num"]=>
  string(2) "44"
}
object(GMP)#%d (1) {
  ["num"]=>
  string(2) "43"
}
object(GMP)#%d (1) {
  ["num"]=>
  string(2) "43"
}
object(GMP)#%d (1) {
  ["num"]=>
  string(2) "42"
}
string(4) "4217"
string(4) "4217"
string(4) "4217"
string(4) "4217"
