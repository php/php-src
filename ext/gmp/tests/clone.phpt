--TEST--
Cloning GMP instances
--EXTENSIONS--
gmp
--FILE--
<?php

$a = gmp_init(3);
$b = clone $a;
gmp_clrbit($a, 0);
var_dump($a, $b); // $b should be unaffected

?>
--EXPECT--
object(GMP)#1 (1) {
  ["num"]=>
  string(1) "2"
}
object(GMP)#2 (1) {
  ["num"]=>
  string(1) "3"
}
