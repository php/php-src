--TEST--
BcMath\Number add int by operator
--EXTENSIONS--
bcmath
--FILE--
<?php

$values = [
    100,
    '-20',
];

foreach ($values as $value1) {
    $num1 = new BcMath\Number($value1);

    foreach ($values as $value2) {
        echo "{$value1} + {$value2}\n";
        $ret = $num1 + ((int) $value2);
        $ret2 = ((int) $value1) + (new BcMath\Number($value2));
        if ($ret->compare($ret2) !== 0) {
            echo "Result is incorrect.\n";
        }
        var_dump($ret);
        echo "\n";
    }
}
?>
--EXPECT--
100 + 100
object(BcMath\Number)#2 (2) {
  ["value"]=>
  string(3) "200"
  ["scale"]=>
  int(0)
}

100 + -20
object(BcMath\Number)#3 (2) {
  ["value"]=>
  string(2) "80"
  ["scale"]=>
  int(0)
}

-20 + 100
object(BcMath\Number)#1 (2) {
  ["value"]=>
  string(2) "80"
  ["scale"]=>
  int(0)
}

-20 + -20
object(BcMath\Number)#5 (2) {
  ["value"]=>
  string(3) "-40"
  ["scale"]=>
  int(0)
}
