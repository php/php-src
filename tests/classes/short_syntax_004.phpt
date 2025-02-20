--TEST--
auto-public properties
--FILE--
<?php

class Point(int $x, int $y);

$p = new Point(1, 2);
var_dump($p->x);
var_dump($p->y);
?>
--EXPECT--
int(1)
int(2)
