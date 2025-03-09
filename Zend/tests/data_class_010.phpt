--TEST--
Serialization works
--FILE--
<?php

data class Point {
    public function __construct(public int $x, public int $y) {}
}

$p = new Point(1, 2);
$s = serialize($p);
var_dump($s);
$p2 = unserialize($s);
var_dump($p2 === $p);
?>
--EXPECT--
string(40) "O:5:"Point":2:{s:1:"x";i:1;s:1:"y";i:2;}"
bool(true)
