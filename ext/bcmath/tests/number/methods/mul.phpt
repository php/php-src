--TEST--
BcMath\Number mul()
--EXTENSIONS--
bcmath
--FILE--
<?php

$values1 = ['100.012', '-100.012'];

$values2 = [
    [100, 'int'],
    [-30, 'int'],
    ['-20', 'string'],
    ['0.01', 'string'],
    ['-0.40', 'string'],
    [new BcMath\Number('80.3'), 'object'],
    [new BcMath\Number('-50.6'), 'object'],
];

foreach ($values1 as $value1) {
    $num = new BcMath\Number($value1);

    foreach ($values2 as [$value2, $type]) {
        echo "{$value1} * {$value2}: {$type}\n";
        $ret = $num->mul($value2);
        var_dump($ret);
        echo "\n";
    }
}
?>
--EXPECT--
100.012 * 100: int
object(BcMath\Number)#4 (2) {
  ["value"]=>
  string(9) "10001.200"
  ["scale"]=>
  int(3)
}

100.012 * -30: int
object(BcMath\Number)#5 (2) {
  ["value"]=>
  string(9) "-3000.360"
  ["scale"]=>
  int(3)
}

100.012 * -20: string
object(BcMath\Number)#4 (2) {
  ["value"]=>
  string(9) "-2000.240"
  ["scale"]=>
  int(3)
}

100.012 * 0.01: string
object(BcMath\Number)#5 (2) {
  ["value"]=>
  string(7) "1.00012"
  ["scale"]=>
  int(5)
}

100.012 * -0.40: string
object(BcMath\Number)#4 (2) {
  ["value"]=>
  string(9) "-40.00480"
  ["scale"]=>
  int(5)
}

100.012 * 80.3: object
object(BcMath\Number)#5 (2) {
  ["value"]=>
  string(9) "8030.9636"
  ["scale"]=>
  int(4)
}

100.012 * -50.6: object
object(BcMath\Number)#4 (2) {
  ["value"]=>
  string(10) "-5060.6072"
  ["scale"]=>
  int(4)
}

-100.012 * 100: int
object(BcMath\Number)#3 (2) {
  ["value"]=>
  string(10) "-10001.200"
  ["scale"]=>
  int(3)
}

-100.012 * -30: int
object(BcMath\Number)#4 (2) {
  ["value"]=>
  string(8) "3000.360"
  ["scale"]=>
  int(3)
}

-100.012 * -20: string
object(BcMath\Number)#3 (2) {
  ["value"]=>
  string(8) "2000.240"
  ["scale"]=>
  int(3)
}

-100.012 * 0.01: string
object(BcMath\Number)#4 (2) {
  ["value"]=>
  string(8) "-1.00012"
  ["scale"]=>
  int(5)
}

-100.012 * -0.40: string
object(BcMath\Number)#3 (2) {
  ["value"]=>
  string(8) "40.00480"
  ["scale"]=>
  int(5)
}

-100.012 * 80.3: object
object(BcMath\Number)#4 (2) {
  ["value"]=>
  string(10) "-8030.9636"
  ["scale"]=>
  int(4)
}

-100.012 * -50.6: object
object(BcMath\Number)#3 (2) {
  ["value"]=>
  string(9) "5060.6072"
  ["scale"]=>
  int(4)
}
