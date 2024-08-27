--TEST--
BcMath\Number construct
--EXTENSIONS--
bcmath
--FILE--
<?php

$values = [
    '0',
    '0.00',
    '.1',
    '0.1',
    '0.20',
    '0.00000030',
    '1234.0',
    '123450',
    '',
    '-',
    '.',
    0,
    123,
];

foreach ($values as $value) {
    $num = new BcMath\Number($value);
    var_dump($num);
    unset($num);
}
?>
--EXPECT--
object(BcMath\Number)#1 (2) {
  ["value"]=>
  string(1) "0"
  ["scale"]=>
  int(0)
}
object(BcMath\Number)#1 (2) {
  ["value"]=>
  string(4) "0.00"
  ["scale"]=>
  int(2)
}
object(BcMath\Number)#1 (2) {
  ["value"]=>
  string(3) "0.1"
  ["scale"]=>
  int(1)
}
object(BcMath\Number)#1 (2) {
  ["value"]=>
  string(3) "0.1"
  ["scale"]=>
  int(1)
}
object(BcMath\Number)#1 (2) {
  ["value"]=>
  string(4) "0.20"
  ["scale"]=>
  int(2)
}
object(BcMath\Number)#1 (2) {
  ["value"]=>
  string(10) "0.00000030"
  ["scale"]=>
  int(8)
}
object(BcMath\Number)#1 (2) {
  ["value"]=>
  string(6) "1234.0"
  ["scale"]=>
  int(1)
}
object(BcMath\Number)#1 (2) {
  ["value"]=>
  string(6) "123450"
  ["scale"]=>
  int(0)
}
object(BcMath\Number)#1 (2) {
  ["value"]=>
  string(1) "0"
  ["scale"]=>
  int(0)
}
object(BcMath\Number)#1 (2) {
  ["value"]=>
  string(1) "0"
  ["scale"]=>
  int(0)
}
object(BcMath\Number)#1 (2) {
  ["value"]=>
  string(1) "0"
  ["scale"]=>
  int(0)
}
object(BcMath\Number)#1 (2) {
  ["value"]=>
  string(1) "0"
  ["scale"]=>
  int(0)
}
object(BcMath\Number)#1 (2) {
  ["value"]=>
  string(3) "123"
  ["scale"]=>
  int(0)
}
