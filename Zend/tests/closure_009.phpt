--TEST--
Closure 009: Using static vars inside lambda
--FILE--
<?php
$a = 1;
$x = function ($x) use ($a) {
  static $n = 0;
  $n++;
  $a = $n.':'.$a;
  echo $x.':'.$a."\n";
};
$y = function ($x) use (&$a) {
  static $n = 0;
  $n++;
  $a = $n.':'.$a;
  echo $x.':'.$a."\n";
};
$x(1);
$x(2);
$x(3);
$y(4);
$y(5);
$y(6);
?>
--EXPECT--
1:1:1
2:2:1
3:3:1
4:1:1
5:2:1:1
6:3:2:1:1
