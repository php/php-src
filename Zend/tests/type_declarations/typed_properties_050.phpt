--TEST--
Weak casts must not overwrite source variables
--FILE--
<?php

$b = 1;
$a = "$b";
class A { public int $a; }
$o = new A;
$o->a = $b;
var_dump($o, $a);

?>
--EXPECT--
object(A)#1 (1) {
  ["a"]=>
  int(1)
}
string(1) "1"
