--TEST--
BcMath\Number decrement
--EXTENSIONS--
bcmath
--FILE--
<?php

$values = [
    100,
    '-20',
    '0.01',
    '-0.40',
];

foreach ($values as $value) {
    echo "========== {$value}-- ==========\n";
    $num = new BcMath\Number($value);
    $num_old = $num;

    $num--;
    echo '$num:' . "\n";
    var_dump($num);

    echo '$num_old:' . "\n";
    var_dump($num_old);

    echo "\n";
}
?>
--EXPECT--
========== 100-- ==========
$num:
object(BcMath\Number)#2 (2) {
  ["value"]=>
  string(2) "99"
  ["scale"]=>
  int(0)
}
$num_old:
object(BcMath\Number)#1 (2) {
  ["value"]=>
  string(3) "100"
  ["scale"]=>
  int(0)
}

========== -20-- ==========
$num:
object(BcMath\Number)#1 (2) {
  ["value"]=>
  string(3) "-21"
  ["scale"]=>
  int(0)
}
$num_old:
object(BcMath\Number)#3 (2) {
  ["value"]=>
  string(3) "-20"
  ["scale"]=>
  int(0)
}

========== 0.01-- ==========
$num:
object(BcMath\Number)#3 (2) {
  ["value"]=>
  string(5) "-0.99"
  ["scale"]=>
  int(2)
}
$num_old:
object(BcMath\Number)#2 (2) {
  ["value"]=>
  string(4) "0.01"
  ["scale"]=>
  int(2)
}

========== -0.40-- ==========
$num:
object(BcMath\Number)#2 (2) {
  ["value"]=>
  string(5) "-1.40"
  ["scale"]=>
  int(2)
}
$num_old:
object(BcMath\Number)#1 (2) {
  ["value"]=>
  string(5) "-0.40"
  ["scale"]=>
  int(2)
}
