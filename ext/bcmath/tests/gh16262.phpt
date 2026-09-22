--TEST--
GH-16262 Stack buffer overflow in ext/bcmath/libbcmath/src/div.c:459
--EXTENSIONS--
bcmath
--INI--
bcmath.scale=0
--FILE--
<?php
$scales = [
    null,
    0,
    1,
    2,
    3,
    4,
    5,
];
foreach ($scales as $scale) {
    echo '========== scale: ', $scale ?? 'null', " ==========\n";
    echo "1 / 1000:\n";
    var_dump(
        bcdiv('1', '1000', $scale),
        (new BcMath\Number('1'))->div('1000', $scale),
    );
    echo "1 / 2000:\n";
    var_dump(
        bcdiv('1', '2000', $scale),
        (new BcMath\Number('1'))->div('2000', $scale),
    );
    echo "\n";
}
?>
--EXPECT--
========== scale: null ==========
1 / 1000:
string(1) "0"
object(BcMath\Number)#2 (2) {
  ["value"]=>
  string(5) "0.001"
  ["scale"]=>
  int(3)
}
1 / 2000:
string(1) "0"
object(BcMath\Number)#1 (2) {
  ["value"]=>
  string(6) "0.0005"
  ["scale"]=>
  int(4)
}

========== scale: 0 ==========
1 / 1000:
string(1) "0"
object(BcMath\Number)#2 (2) {
  ["value"]=>
  string(1) "0"
  ["scale"]=>
  int(0)
}
1 / 2000:
string(1) "0"
object(BcMath\Number)#1 (2) {
  ["value"]=>
  string(1) "0"
  ["scale"]=>
  int(0)
}

========== scale: 1 ==========
1 / 1000:
string(3) "0.0"
object(BcMath\Number)#2 (2) {
  ["value"]=>
  string(3) "0.0"
  ["scale"]=>
  int(1)
}
1 / 2000:
string(3) "0.0"
object(BcMath\Number)#1 (2) {
  ["value"]=>
  string(3) "0.0"
  ["scale"]=>
  int(1)
}

========== scale: 2 ==========
1 / 1000:
string(4) "0.00"
object(BcMath\Number)#2 (2) {
  ["value"]=>
  string(4) "0.00"
  ["scale"]=>
  int(2)
}
1 / 2000:
string(4) "0.00"
object(BcMath\Number)#1 (2) {
  ["value"]=>
  string(4) "0.00"
  ["scale"]=>
  int(2)
}

========== scale: 3 ==========
1 / 1000:
string(5) "0.001"
object(BcMath\Number)#2 (2) {
  ["value"]=>
  string(5) "0.001"
  ["scale"]=>
  int(3)
}
1 / 2000:
string(5) "0.000"
object(BcMath\Number)#1 (2) {
  ["value"]=>
  string(5) "0.000"
  ["scale"]=>
  int(3)
}

========== scale: 4 ==========
1 / 1000:
string(6) "0.0010"
object(BcMath\Number)#2 (2) {
  ["value"]=>
  string(6) "0.0010"
  ["scale"]=>
  int(4)
}
1 / 2000:
string(6) "0.0005"
object(BcMath\Number)#1 (2) {
  ["value"]=>
  string(6) "0.0005"
  ["scale"]=>
  int(4)
}

========== scale: 5 ==========
1 / 1000:
string(7) "0.00100"
object(BcMath\Number)#2 (2) {
  ["value"]=>
  string(7) "0.00100"
  ["scale"]=>
  int(5)
}
1 / 2000:
string(7) "0.00050"
object(BcMath\Number)#1 (2) {
  ["value"]=>
  string(7) "0.00050"
  ["scale"]=>
  int(5)
}
