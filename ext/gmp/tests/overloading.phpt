--TEST--
GMP operator overloading
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

var_dump($a / $b);
var_dump($a / 17);
var_dump(42 / $b);
var_dump($a / 0);

var_dump($a % $b);
var_dump($a % 17);
var_dump(42 % $b);
var_dump($a % 0);

// sl, sr

var_dump($a | $b);
var_dump($a | 17);
var_dump(42 | $b);

var_dump($a & $b);
var_dump($a & 17);
var_dump(42 & $b);

var_dump($a ^ $b);
var_dump($a ^ 17);
var_dump(42 ^ $b);

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

