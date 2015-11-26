--TEST--
Bug #54970 (SplFixedArray::setSize() isn't resizing)
--FILE--
<?php

$fa = new SplFixedArray(2);
$fa[0] = 'Hello';
$fa[1] = 'World';
$fa->setSize(3);
$fa[2] = '!';
var_dump($fa);
$fa->setSize(2);
var_dump($fa);
var_dump($fa->getSize());


?>
--EXPECTF--
object(SplFixedArray)#%d (3) {
  [0]=>
  string(5) "Hello"
  [1]=>
  string(5) "World"
  [2]=>
  string(1) "!"
}
object(SplFixedArray)#%d (2) {
  [0]=>
  string(5) "Hello"
  [1]=>
  string(5) "World"
}
int(2)
