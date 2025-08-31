--TEST--
BcMath\Number powmod()
--EXTENSIONS--
bcmath
--FILE--
<?php

$values = ['12', '-12'];

$exponents = [
    [2, 'int'],
    ['0', 'string'],
    ['3', 'string'],
    [new BcMath\Number('2'), 'object'],
    [new BcMath\Number('0'), 'object'],
];

$mods = [
    [2, 'int'],
    ['3', 'string'],
    [new BcMath\Number('2'), 'object'],
];

foreach ($values as $value) {
    $num = new BcMath\Number($value);

    foreach ($exponents as [$exponent, $type_ex]) {
        foreach ($mods as [$mod, $type_mod]) {
            echo "{$value}, expo is {$exponent}({$type_ex}), mod is {$mod}({$type_mod}):\n";
            $ret = $num->powmod($exponent, $mod);
            var_dump($ret);
            echo "\n";
        }
    }
}
?>
--EXPECT--
12, expo is 2(int), mod is 2(int):
object(BcMath\Number)#5 (2) {
  ["value"]=>
  string(1) "0"
  ["scale"]=>
  int(0)
}

12, expo is 2(int), mod is 3(string):
object(BcMath\Number)#6 (2) {
  ["value"]=>
  string(1) "0"
  ["scale"]=>
  int(0)
}

12, expo is 2(int), mod is 2(object):
object(BcMath\Number)#5 (2) {
  ["value"]=>
  string(1) "0"
  ["scale"]=>
  int(0)
}

12, expo is 0(string), mod is 2(int):
object(BcMath\Number)#6 (2) {
  ["value"]=>
  string(1) "1"
  ["scale"]=>
  int(0)
}

12, expo is 0(string), mod is 3(string):
object(BcMath\Number)#5 (2) {
  ["value"]=>
  string(1) "1"
  ["scale"]=>
  int(0)
}

12, expo is 0(string), mod is 2(object):
object(BcMath\Number)#6 (2) {
  ["value"]=>
  string(1) "1"
  ["scale"]=>
  int(0)
}

12, expo is 3(string), mod is 2(int):
object(BcMath\Number)#5 (2) {
  ["value"]=>
  string(1) "0"
  ["scale"]=>
  int(0)
}

12, expo is 3(string), mod is 3(string):
object(BcMath\Number)#6 (2) {
  ["value"]=>
  string(1) "0"
  ["scale"]=>
  int(0)
}

12, expo is 3(string), mod is 2(object):
object(BcMath\Number)#5 (2) {
  ["value"]=>
  string(1) "0"
  ["scale"]=>
  int(0)
}

12, expo is 2(object), mod is 2(int):
object(BcMath\Number)#6 (2) {
  ["value"]=>
  string(1) "0"
  ["scale"]=>
  int(0)
}

12, expo is 2(object), mod is 3(string):
object(BcMath\Number)#5 (2) {
  ["value"]=>
  string(1) "0"
  ["scale"]=>
  int(0)
}

12, expo is 2(object), mod is 2(object):
object(BcMath\Number)#6 (2) {
  ["value"]=>
  string(1) "0"
  ["scale"]=>
  int(0)
}

12, expo is 0(object), mod is 2(int):
object(BcMath\Number)#5 (2) {
  ["value"]=>
  string(1) "1"
  ["scale"]=>
  int(0)
}

12, expo is 0(object), mod is 3(string):
object(BcMath\Number)#6 (2) {
  ["value"]=>
  string(1) "1"
  ["scale"]=>
  int(0)
}

12, expo is 0(object), mod is 2(object):
object(BcMath\Number)#5 (2) {
  ["value"]=>
  string(1) "1"
  ["scale"]=>
  int(0)
}

-12, expo is 2(int), mod is 2(int):
object(BcMath\Number)#4 (2) {
  ["value"]=>
  string(1) "0"
  ["scale"]=>
  int(0)
}

-12, expo is 2(int), mod is 3(string):
object(BcMath\Number)#5 (2) {
  ["value"]=>
  string(1) "0"
  ["scale"]=>
  int(0)
}

-12, expo is 2(int), mod is 2(object):
object(BcMath\Number)#4 (2) {
  ["value"]=>
  string(1) "0"
  ["scale"]=>
  int(0)
}

-12, expo is 0(string), mod is 2(int):
object(BcMath\Number)#5 (2) {
  ["value"]=>
  string(1) "1"
  ["scale"]=>
  int(0)
}

-12, expo is 0(string), mod is 3(string):
object(BcMath\Number)#4 (2) {
  ["value"]=>
  string(1) "1"
  ["scale"]=>
  int(0)
}

-12, expo is 0(string), mod is 2(object):
object(BcMath\Number)#5 (2) {
  ["value"]=>
  string(1) "1"
  ["scale"]=>
  int(0)
}

-12, expo is 3(string), mod is 2(int):
object(BcMath\Number)#4 (2) {
  ["value"]=>
  string(1) "0"
  ["scale"]=>
  int(0)
}

-12, expo is 3(string), mod is 3(string):
object(BcMath\Number)#5 (2) {
  ["value"]=>
  string(1) "0"
  ["scale"]=>
  int(0)
}

-12, expo is 3(string), mod is 2(object):
object(BcMath\Number)#4 (2) {
  ["value"]=>
  string(1) "0"
  ["scale"]=>
  int(0)
}

-12, expo is 2(object), mod is 2(int):
object(BcMath\Number)#5 (2) {
  ["value"]=>
  string(1) "0"
  ["scale"]=>
  int(0)
}

-12, expo is 2(object), mod is 3(string):
object(BcMath\Number)#4 (2) {
  ["value"]=>
  string(1) "0"
  ["scale"]=>
  int(0)
}

-12, expo is 2(object), mod is 2(object):
object(BcMath\Number)#5 (2) {
  ["value"]=>
  string(1) "0"
  ["scale"]=>
  int(0)
}

-12, expo is 0(object), mod is 2(int):
object(BcMath\Number)#4 (2) {
  ["value"]=>
  string(1) "1"
  ["scale"]=>
  int(0)
}

-12, expo is 0(object), mod is 3(string):
object(BcMath\Number)#5 (2) {
  ["value"]=>
  string(1) "1"
  ["scale"]=>
  int(0)
}

-12, expo is 0(object), mod is 2(object):
object(BcMath\Number)#4 (2) {
  ["value"]=>
  string(1) "1"
  ["scale"]=>
  int(0)
}
