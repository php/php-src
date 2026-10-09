--TEST--
List assignment from an array literal: values and targets
--FILE--
<?php

$a = 1; $b = 2; $c = 3;
[$a, $b] = [$b, $a];
var_dump($a, $b);

[$a, $b, $c] = [$b, $c, $a];
var_dump($a, $b, $c);

list($a, $b) = array($b, $a);
var_dump($a, $b);

[$a, $a] = [1, 2];
var_dump($a);

$a = 1;
[$a, $b] = [2, $a];
var_dump($a, $b);

[$a, $b,] = [$b, $a,];
var_dump($a, $b);

class P { public $a = 1; public $b = 2; public static $c = 3; public static $d = 4; }
$p = new P;
[$p->a, $p->b] = [$p->b, $p->a];
[P::$c, P::$d] = [P::$d, P::$c];
var_dump($p->a, $p->b, P::$c, P::$d);

$x = [1, 2];
[$x[0], $x[1]] = [$x[1], $x[0]];
var_dump($x);

[$GLOBALS['g']] = [5];
var_dump($g);

$r = [$a, $b] = [7, 8];
var_dump($r, $a, $b);

$a = 1;
[$f, $a] = [fn() => $a, 2];
var_dump($f());

for ([$a, $b] = [1, 2], $i = 0; $i < 3; $i++, [$a, $b] = [$b, $a + $b]) {
    echo "$a $b\n";
}
for ($i = 0; [$a, $b] = [$b, $a], $i < 2; $i++) {
    echo "$a $b\n";
}
for ($i = 0; [$x, $y] = [$i, $i * 2]; $i++) {
    if ($i > 1) break;
    echo "$x $y\n";
}

?>
--EXPECT--
int(2)
int(1)
int(1)
int(3)
int(2)
int(3)
int(1)
int(2)
int(2)
int(1)
int(1)
int(2)
int(2)
int(1)
int(4)
int(3)
array(2) {
  [0]=>
  int(2)
  [1]=>
  int(1)
}
int(5)
array(2) {
  [0]=>
  int(7)
  [1]=>
  int(8)
}
int(7)
int(8)
int(1)
1 2
2 3
3 5
8 5
5 8
0 0
1 2
