--TEST--
GMP serialization and unserialization via magic methods
--EXTENSIONS--
gmp
--FILE--
<?php

$n = gmp_init(13);
var_dump($n);
$a = $n->__serialize();
var_dump($a);

$n -= 11;
var_dump($n);
$n->__unserialize($a);
var_dump($n);

?>
--EXPECT--
object(GMP)#1 (1) {
  ["num"]=>
  string(2) "13"
}
array(1) {
  [0]=>
  string(1) "d"
}
object(GMP)#2 (1) {
  ["num"]=>
  string(1) "2"
}
object(GMP)#2 (1) {
  ["num"]=>
  string(2) "13"
}
