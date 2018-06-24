--TEST--
__compareTo: Supported by array functions with ordering semantics (sorting)
--FILE--
<?php
include("Comparable.inc");

$a = new Comparable(3, 1); // third
$b = new Comparable(1, 2); // first
$c = new Comparable(2, 3); // second

$array = [$a, $b, $c];

sort($array);

var_dump($array == [$b, $c, $a]);

?>
--EXPECTF--
bool(true)
