--TEST--
BcMath\Number divmod()
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
        echo "{$value1} divmod {$value2}: {$type}\n";
        [$quot, $rem] = $num->divmod($value2);
        var_dump($quot, $rem);
        echo "\n";
    }
}
?>
--EXPECT--
100.012 divmod 100: int
object(BcMath\Number)#4 (2) {
  ["value"]=>
  string(1) "1"
  ["scale"]=>
  int(0)
}
object(BcMath\Number)#5 (2) {
  ["value"]=>
  string(5) "0.012"
  ["scale"]=>
  int(3)
}

100.012 divmod -30: int
object(BcMath\Number)#6 (2) {
  ["value"]=>
  string(2) "-3"
  ["scale"]=>
  int(0)
}
object(BcMath\Number)#7 (2) {
  ["value"]=>
  string(6) "10.012"
  ["scale"]=>
  int(3)
}

100.012 divmod -20: string
object(BcMath\Number)#5 (2) {
  ["value"]=>
  string(2) "-5"
  ["scale"]=>
  int(0)
}
object(BcMath\Number)#4 (2) {
  ["value"]=>
  string(5) "0.012"
  ["scale"]=>
  int(3)
}

100.012 divmod 0.01: string
object(BcMath\Number)#7 (2) {
  ["value"]=>
  string(5) "10001"
  ["scale"]=>
  int(0)
}
object(BcMath\Number)#6 (2) {
  ["value"]=>
  string(5) "0.002"
  ["scale"]=>
  int(3)
}

100.012 divmod -0.40: string
object(BcMath\Number)#4 (2) {
  ["value"]=>
  string(4) "-250"
  ["scale"]=>
  int(0)
}
object(BcMath\Number)#5 (2) {
  ["value"]=>
  string(5) "0.012"
  ["scale"]=>
  int(3)
}

100.012 divmod 80.3: object
object(BcMath\Number)#6 (2) {
  ["value"]=>
  string(1) "1"
  ["scale"]=>
  int(0)
}
object(BcMath\Number)#7 (2) {
  ["value"]=>
  string(6) "19.712"
  ["scale"]=>
  int(3)
}

100.012 divmod -50.6: object
object(BcMath\Number)#5 (2) {
  ["value"]=>
  string(2) "-1"
  ["scale"]=>
  int(0)
}
object(BcMath\Number)#4 (2) {
  ["value"]=>
  string(6) "49.412"
  ["scale"]=>
  int(3)
}

-100.012 divmod 100: int
object(BcMath\Number)#3 (2) {
  ["value"]=>
  string(2) "-1"
  ["scale"]=>
  int(0)
}
object(BcMath\Number)#6 (2) {
  ["value"]=>
  string(6) "-0.012"
  ["scale"]=>
  int(3)
}

-100.012 divmod -30: int
object(BcMath\Number)#4 (2) {
  ["value"]=>
  string(1) "3"
  ["scale"]=>
  int(0)
}
object(BcMath\Number)#5 (2) {
  ["value"]=>
  string(7) "-10.012"
  ["scale"]=>
  int(3)
}

-100.012 divmod -20: string
object(BcMath\Number)#6 (2) {
  ["value"]=>
  string(1) "5"
  ["scale"]=>
  int(0)
}
object(BcMath\Number)#3 (2) {
  ["value"]=>
  string(6) "-0.012"
  ["scale"]=>
  int(3)
}

-100.012 divmod 0.01: string
object(BcMath\Number)#5 (2) {
  ["value"]=>
  string(6) "-10001"
  ["scale"]=>
  int(0)
}
object(BcMath\Number)#4 (2) {
  ["value"]=>
  string(6) "-0.002"
  ["scale"]=>
  int(3)
}

-100.012 divmod -0.40: string
object(BcMath\Number)#3 (2) {
  ["value"]=>
  string(3) "250"
  ["scale"]=>
  int(0)
}
object(BcMath\Number)#6 (2) {
  ["value"]=>
  string(6) "-0.012"
  ["scale"]=>
  int(3)
}

-100.012 divmod 80.3: object
object(BcMath\Number)#4 (2) {
  ["value"]=>
  string(2) "-1"
  ["scale"]=>
  int(0)
}
object(BcMath\Number)#5 (2) {
  ["value"]=>
  string(7) "-19.712"
  ["scale"]=>
  int(3)
}

-100.012 divmod -50.6: object
object(BcMath\Number)#6 (2) {
  ["value"]=>
  string(1) "1"
  ["scale"]=>
  int(0)
}
object(BcMath\Number)#3 (2) {
  ["value"]=>
  string(7) "-49.412"
  ["scale"]=>
  int(3)
}
