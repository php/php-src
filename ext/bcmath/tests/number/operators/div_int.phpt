--TEST--
BcMath\Number div int by operator
--EXTENSIONS--
bcmath
--FILE--
<?php

$values = [
    100,
    '-20',
    3,
];

foreach ($values as $value1) {
    $num1 = new BcMath\Number($value1);

    foreach ($values as $value2) {
        echo "{$value1} / {$value2}\n";
        $ret = $num1 / ((int) $value2);
        $ret2 = ((int) $value1) / (new BcMath\Number($value2));
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

100 / 3
object(BcMath\Number)#4 (2) {
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
object(BcMath\Number)#2 (2) {
  ["value"]=>
  string(1) "1"
  ["scale"]=>
  int(0)
}

-20 / 3
object(BcMath\Number)#3 (2) {
  ["value"]=>
  string(13) "-6.6666666666"
  ["scale"]=>
  int(10)
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

3 / 3
object(BcMath\Number)#2 (2) {
  ["value"]=>
  string(1) "1"
  ["scale"]=>
  int(0)
}
