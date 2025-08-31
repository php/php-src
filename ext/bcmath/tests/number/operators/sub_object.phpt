--TEST--
BcMath\Number sub object by operator
--EXTENSIONS--
bcmath
--FILE--
<?php

$values = [
    100,
    '20',
    '0.01',
    '0.40',
];

foreach ($values as $value1) {
    $num1 = new BcMath\Number($value1);

    foreach ($values as $value2) {
        echo "{$value1} - {$value2}\n";
        $num2 = new BcMath\Number($value2);
        $ret = $num1 - $num2;
        var_dump($ret);
        echo "\n";
    }
}
?>
--EXPECT--
100 - 100
object(BcMath\Number)#3 (2) {
  ["value"]=>
  string(1) "0"
  ["scale"]=>
  int(0)
}

100 - 20
object(BcMath\Number)#2 (2) {
  ["value"]=>
  string(2) "80"
  ["scale"]=>
  int(0)
}

100 - 0.01
object(BcMath\Number)#4 (2) {
  ["value"]=>
  string(5) "99.99"
  ["scale"]=>
  int(2)
}

100 - 0.40
object(BcMath\Number)#3 (2) {
  ["value"]=>
  string(5) "99.60"
  ["scale"]=>
  int(2)
}

20 - 100
object(BcMath\Number)#2 (2) {
  ["value"]=>
  string(3) "-80"
  ["scale"]=>
  int(0)
}

20 - 20
object(BcMath\Number)#1 (2) {
  ["value"]=>
  string(1) "0"
  ["scale"]=>
  int(0)
}

20 - 0.01
object(BcMath\Number)#3 (2) {
  ["value"]=>
  string(5) "19.99"
  ["scale"]=>
  int(2)
}

20 - 0.40
object(BcMath\Number)#2 (2) {
  ["value"]=>
  string(5) "19.60"
  ["scale"]=>
  int(2)
}

0.01 - 100
object(BcMath\Number)#1 (2) {
  ["value"]=>
  string(6) "-99.99"
  ["scale"]=>
  int(2)
}

0.01 - 20
object(BcMath\Number)#4 (2) {
  ["value"]=>
  string(6) "-19.99"
  ["scale"]=>
  int(2)
}

0.01 - 0.01
object(BcMath\Number)#2 (2) {
  ["value"]=>
  string(4) "0.00"
  ["scale"]=>
  int(2)
}

0.01 - 0.40
object(BcMath\Number)#1 (2) {
  ["value"]=>
  string(5) "-0.39"
  ["scale"]=>
  int(2)
}

0.40 - 100
object(BcMath\Number)#4 (2) {
  ["value"]=>
  string(6) "-99.60"
  ["scale"]=>
  int(2)
}

0.40 - 20
object(BcMath\Number)#3 (2) {
  ["value"]=>
  string(6) "-19.60"
  ["scale"]=>
  int(2)
}

0.40 - 0.01
object(BcMath\Number)#1 (2) {
  ["value"]=>
  string(4) "0.39"
  ["scale"]=>
  int(2)
}

0.40 - 0.40
object(BcMath\Number)#4 (2) {
  ["value"]=>
  string(4) "0.00"
  ["scale"]=>
  int(2)
}
