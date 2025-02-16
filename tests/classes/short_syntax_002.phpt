--TEST--
Simple parameters
--FILE--
<?php

class Point(public int $x, public int $y);

$p = new Point(1, 2);

var_dump($p);
?>
--EXPECT--
object(Point)#1 (2) {
  ["x"]=>
  int(1)
  ["y"]=>
  int(2)
}
