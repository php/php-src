--TEST--
Overloaded GMP comparison in sort() etc
--SKIPIF--
<?php if (!extension_loaded("gmp")) print "skip"; ?>
--FILE--
<?php

$arr = [gmp_init(0), -3, gmp_init(2), 1];
sort($arr);
var_dump($arr);

var_dump(min(gmp_init(3), 4));
var_dump(max(gmp_init(3), 4));

?>
--EXPECT--
array(4) {
  [0]=>
  int(-3)
  [1]=>
  object(GMP)#1 (1) {
    ["num"]=>
    string(1) "0"
  }
  [2]=>
  int(1)
  [3]=>
  object(GMP)#2 (1) {
    ["num"]=>
    string(1) "2"
  }
}
object(GMP)#3 (1) {
  ["num"]=>
  string(1) "3"
}
int(4)
