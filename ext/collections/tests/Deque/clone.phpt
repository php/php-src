--TEST--
Collections\Deque can be cloned after building properties table.
--FILE--
<?php
$x = new Collections\Deque([new stdClass()]);
var_dump($x);
$y = clone $x;
var_dump($y);
?>
--EXPECT--
object(Collections\Deque)#1 (1) {
  [0]=>
  object(stdClass)#2 (0) {
  }
}
object(Collections\Deque)#3 (1) {
  [0]=>
  object(stdClass)#2 (0) {
  }
}
