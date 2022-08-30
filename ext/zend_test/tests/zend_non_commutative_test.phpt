--TEST--
Zend: test non commutative multiplication object handler
--EXTENSIONS--
zend_test
--FILE--
<?php

$ten = new NonCommutativeMultiplication(10);
$fifty = new NonCommutativeMultiplication(50);
$hundred = new NonCommutativeMultiplication(100);

var_dump($ten);
var_dump($fifty);
var_dump($hundred);

/* $hundred * 20 gives a new NCM object, thus final result is 30 = ((100 - 20) - 50) */
var_dump($hundred * 20 * 50);
/* Multiplication corresponds to subtraction for this object */
var_dump(100 - 50 - 10);
var_dump($hundred * $fifty * $ten);
var_dump(10 - 50 - 100);
var_dump($ten * $fifty * $hundred);
var_dump(50 - 100 - 10);
var_dump($fifty * $hundred * $ten);


?>
--EXPECT--
object(NonCommutativeMultiplication)#1 (1) {
  ["val":"NonCommutativeMultiplication":private]=>
  int(10)
}
object(NonCommutativeMultiplication)#2 (1) {
  ["val":"NonCommutativeMultiplication":private]=>
  int(50)
}
object(NonCommutativeMultiplication)#3 (1) {
  ["val":"NonCommutativeMultiplication":private]=>
  int(100)
}
object(NonCommutativeMultiplication)#5 (1) {
  ["val":"NonCommutativeMultiplication":private]=>
  int(30)
}
int(40)
object(NonCommutativeMultiplication)#4 (1) {
  ["val":"NonCommutativeMultiplication":private]=>
  int(40)
}
int(-140)
object(NonCommutativeMultiplication)#5 (1) {
  ["val":"NonCommutativeMultiplication":private]=>
  int(-140)
}
int(-60)
object(NonCommutativeMultiplication)#4 (1) {
  ["val":"NonCommutativeMultiplication":private]=>
  int(-60)
}
