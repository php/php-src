--TEST--
BcMath\Number div string by operator
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

foreach ($values as $value1) {
    $num1 = new BcMath\Number($value1);

    foreach ($values as $value2) {
        echo "{$value1} / {$value2}\n";
        $ret = $num1 / ((string) $value2);
        $ret2 = ((string) $value1) / (new BcMath\Number($value2));
        if ($ret->compare($ret2) !== 0) {
            echo "Result is incorrect.\n";
        }
        var_dump($ret);
        echo "\n";
    }
}
?>
--EXPECT--
100 / 100
object(BcMath\Number)#2 (2) {
  ["value"]=>
  string(1) "1"
  ["scale"]=>
  int(0)
}

100 / -20
object(BcMath\Number)#3 (2) {
  ["value"]=>
  string(2) "-5"
  ["scale"]=>
  int(0)
}

100 / 0.01
object(BcMath\Number)#4 (2) {
  ["value"]=>
  string(5) "10000"
  ["scale"]=>
  int(0)
}

100 / -0.40
object(BcMath\Number)#5 (2) {
  ["value"]=>
  string(4) "-250"
  ["scale"]=>
  int(0)
}

100 / 3
object(BcMath\Number)#2 (2) {
  ["value"]=>
  string(13) "33.3333333333"
  ["scale"]=>
  int(10)
}

-20 / 100
object(BcMath\Number)#1 (2) {
  ["value"]=>
  string(4) "-0.2"
  ["scale"]=>
  int(1)
}

-20 / -20
object(BcMath\Number)#4 (2) {
  ["value"]=>
  string(1) "1"
  ["scale"]=>
  int(0)
}

-20 / 0.01
object(BcMath\Number)#5 (2) {
  ["value"]=>
  string(5) "-2000"
  ["scale"]=>
  int(0)
}

-20 / -0.40
object(BcMath\Number)#2 (2) {
  ["value"]=>
  string(2) "50"
  ["scale"]=>
  int(0)
}

-20 / 3
object(BcMath\Number)#1 (2) {
  ["value"]=>
  string(13) "-6.6666666666"
  ["scale"]=>
  int(10)
}

0.01 / 100
object(BcMath\Number)#3 (2) {
  ["value"]=>
  string(6) "0.0001"
  ["scale"]=>
  int(4)
}

0.01 / -20
object(BcMath\Number)#5 (2) {
  ["value"]=>
  string(7) "-0.0005"
  ["scale"]=>
  int(4)
}

0.01 / 0.01
object(BcMath\Number)#2 (2) {
  ["value"]=>
  string(4) "1.00"
  ["scale"]=>
  int(2)
}

0.01 / -0.40
object(BcMath\Number)#1 (2) {
  ["value"]=>
  string(6) "-0.025"
  ["scale"]=>
  int(3)
}

0.01 / 3
object(BcMath\Number)#3 (2) {
  ["value"]=>
  string(14) "0.003333333333"
  ["scale"]=>
  int(12)
}

-0.40 / 100
object(BcMath\Number)#4 (2) {
  ["value"]=>
  string(6) "-0.004"
  ["scale"]=>
  int(3)
}

-0.40 / -20
object(BcMath\Number)#2 (2) {
  ["value"]=>
  string(4) "0.02"
  ["scale"]=>
  int(2)
}

-0.40 / 0.01
object(BcMath\Number)#1 (2) {
  ["value"]=>
  string(6) "-40.00"
  ["scale"]=>
  int(2)
}

-0.40 / -0.40
object(BcMath\Number)#3 (2) {
  ["value"]=>
  string(4) "1.00"
  ["scale"]=>
  int(2)
}

-0.40 / 3
object(BcMath\Number)#4 (2) {
  ["value"]=>
  string(15) "-0.133333333333"
  ["scale"]=>
  int(12)
}

3 / 100
object(BcMath\Number)#5 (2) {
  ["value"]=>
  string(4) "0.03"
  ["scale"]=>
  int(2)
}

3 / -20
object(BcMath\Number)#1 (2) {
  ["value"]=>
  string(5) "-0.15"
  ["scale"]=>
  int(2)
}

3 / 0.01
object(BcMath\Number)#3 (2) {
  ["value"]=>
  string(3) "300"
  ["scale"]=>
  int(0)
}

3 / -0.40
object(BcMath\Number)#4 (2) {
  ["value"]=>
  string(4) "-7.5"
  ["scale"]=>
  int(1)
}

3 / 3
object(BcMath\Number)#5 (2) {
  ["value"]=>
  string(1) "1"
  ["scale"]=>
  int(0)
}
