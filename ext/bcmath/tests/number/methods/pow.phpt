--TEST--
BcMath\Number pow()
--EXTENSIONS--
bcmath
--FILE--
<?php

$values = ['12.5', '-12.5'];

$exponents = [
    [2, 'int'],
    [-3, 'int'],
    ['-2', 'string'],
    ['0', 'string'],
    [new BcMath\Number('2'), 'object'],
    [new BcMath\Number('-2'), 'object'],
    [new BcMath\Number('0'), 'object'],
];

foreach ($values as $value) {
    $num = new BcMath\Number($value);

    foreach ($exponents as [$exponent, $type]) {
        echo "{$value} ** {$exponent}: {$type}\n";
        $ret = $num->pow($exponent);
        var_dump($ret);
        echo "\n";
    }
}
?>
--EXPECT--
12.5 ** 2: int
object(BcMath\Number)#5 (2) {
  ["value"]=>
  string(6) "156.25"
  ["scale"]=>
  int(2)
}

12.5 ** -3: int
object(BcMath\Number)#6 (2) {
  ["value"]=>
  string(8) "0.000512"
  ["scale"]=>
  int(6)
}

12.5 ** -2: string
object(BcMath\Number)#5 (2) {
  ["value"]=>
  string(6) "0.0064"
  ["scale"]=>
  int(4)
}

12.5 ** 0: string
object(BcMath\Number)#6 (2) {
  ["value"]=>
  string(1) "1"
  ["scale"]=>
  int(0)
}

12.5 ** 2: object
object(BcMath\Number)#5 (2) {
  ["value"]=>
  string(6) "156.25"
  ["scale"]=>
  int(2)
}

12.5 ** -2: object
object(BcMath\Number)#6 (2) {
  ["value"]=>
  string(6) "0.0064"
  ["scale"]=>
  int(4)
}

12.5 ** 0: object
object(BcMath\Number)#5 (2) {
  ["value"]=>
  string(1) "1"
  ["scale"]=>
  int(0)
}

-12.5 ** 2: int
object(BcMath\Number)#4 (2) {
  ["value"]=>
  string(6) "156.25"
  ["scale"]=>
  int(2)
}

-12.5 ** -3: int
object(BcMath\Number)#5 (2) {
  ["value"]=>
  string(9) "-0.000512"
  ["scale"]=>
  int(6)
}

-12.5 ** -2: string
object(BcMath\Number)#4 (2) {
  ["value"]=>
  string(6) "0.0064"
  ["scale"]=>
  int(4)
}

-12.5 ** 0: string
object(BcMath\Number)#5 (2) {
  ["value"]=>
  string(1) "1"
  ["scale"]=>
  int(0)
}

-12.5 ** 2: object
object(BcMath\Number)#4 (2) {
  ["value"]=>
  string(6) "156.25"
  ["scale"]=>
  int(2)
}

-12.5 ** -2: object
object(BcMath\Number)#5 (2) {
  ["value"]=>
  string(6) "0.0064"
  ["scale"]=>
  int(4)
}

-12.5 ** 0: object
object(BcMath\Number)#4 (2) {
  ["value"]=>
  string(1) "1"
  ["scale"]=>
  int(0)
}
