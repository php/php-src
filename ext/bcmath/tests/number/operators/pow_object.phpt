--TEST--
BcMath\Number pow object by operator
--EXTENSIONS--
bcmath
--FILE--
<?php

$values = [
    100,
    '-20',
    '0.01',
    '-0.40',
    3,
];

$exponents = [
    2,
    '3',
    0,
    -1,
    -2,
];

foreach ($values as $value) {
    $num1 = new BcMath\Number($value);

    foreach ($exponents as $exponent) {
        echo "{$value} ** {$exponent}\n";
        $num2 = new BcMath\Number($exponent);
        $ret = $num1 ** $num2;
        var_dump($ret);
        echo "\n";
    }
}
?>
--EXPECT--
100 ** 2
object(BcMath\Number)#3 (2) {
  ["value"]=>
  string(5) "10000"
  ["scale"]=>
  int(0)
}

100 ** 3
object(BcMath\Number)#2 (2) {
  ["value"]=>
  string(7) "1000000"
  ["scale"]=>
  int(0)
}

100 ** 0
object(BcMath\Number)#4 (2) {
  ["value"]=>
  string(1) "1"
  ["scale"]=>
  int(0)
}

100 ** -1
object(BcMath\Number)#3 (2) {
  ["value"]=>
  string(4) "0.01"
  ["scale"]=>
  int(2)
}

100 ** -2
object(BcMath\Number)#2 (2) {
  ["value"]=>
  string(6) "0.0001"
  ["scale"]=>
  int(4)
}

-20 ** 2
object(BcMath\Number)#4 (2) {
  ["value"]=>
  string(3) "400"
  ["scale"]=>
  int(0)
}

-20 ** 3
object(BcMath\Number)#1 (2) {
  ["value"]=>
  string(5) "-8000"
  ["scale"]=>
  int(0)
}

-20 ** 0
object(BcMath\Number)#2 (2) {
  ["value"]=>
  string(1) "1"
  ["scale"]=>
  int(0)
}

-20 ** -1
object(BcMath\Number)#4 (2) {
  ["value"]=>
  string(5) "-0.05"
  ["scale"]=>
  int(2)
}

-20 ** -2
object(BcMath\Number)#1 (2) {
  ["value"]=>
  string(6) "0.0025"
  ["scale"]=>
  int(4)
}

0.01 ** 2
object(BcMath\Number)#2 (2) {
  ["value"]=>
  string(6) "0.0001"
  ["scale"]=>
  int(4)
}

0.01 ** 3
object(BcMath\Number)#3 (2) {
  ["value"]=>
  string(8) "0.000001"
  ["scale"]=>
  int(6)
}

0.01 ** 0
object(BcMath\Number)#1 (2) {
  ["value"]=>
  string(1) "1"
  ["scale"]=>
  int(0)
}

0.01 ** -1
object(BcMath\Number)#2 (2) {
  ["value"]=>
  string(6) "100.00"
  ["scale"]=>
  int(2)
}

0.01 ** -2
object(BcMath\Number)#3 (2) {
  ["value"]=>
  string(8) "10000.00"
  ["scale"]=>
  int(2)
}

-0.40 ** 2
object(BcMath\Number)#1 (2) {
  ["value"]=>
  string(6) "0.1600"
  ["scale"]=>
  int(4)
}

-0.40 ** 3
object(BcMath\Number)#4 (2) {
  ["value"]=>
  string(9) "-0.064000"
  ["scale"]=>
  int(6)
}

-0.40 ** 0
object(BcMath\Number)#3 (2) {
  ["value"]=>
  string(1) "1"
  ["scale"]=>
  int(0)
}

-0.40 ** -1
object(BcMath\Number)#1 (2) {
  ["value"]=>
  string(5) "-2.50"
  ["scale"]=>
  int(2)
}

-0.40 ** -2
object(BcMath\Number)#4 (2) {
  ["value"]=>
  string(4) "6.25"
  ["scale"]=>
  int(2)
}

3 ** 2
object(BcMath\Number)#3 (2) {
  ["value"]=>
  string(1) "9"
  ["scale"]=>
  int(0)
}

3 ** 3
object(BcMath\Number)#2 (2) {
  ["value"]=>
  string(2) "27"
  ["scale"]=>
  int(0)
}

3 ** 0
object(BcMath\Number)#4 (2) {
  ["value"]=>
  string(1) "1"
  ["scale"]=>
  int(0)
}

3 ** -1
object(BcMath\Number)#3 (2) {
  ["value"]=>
  string(12) "0.3333333333"
  ["scale"]=>
  int(10)
}

3 ** -2
object(BcMath\Number)#2 (2) {
  ["value"]=>
  string(12) "0.1111111111"
  ["scale"]=>
  int(10)
}
