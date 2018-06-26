--TEST--
__equals: Equal values automatically have equal ordering
--FILE--
<?php
include("setup.inc");

$a = new Equatable(1);
$b = new Equatable(1);

/* Give $a a value that $b doesn't have so that default comparison would break. */
$a->unique = 10;

var_dump($a <=> $b);   

?>
--EXPECTF--
int(0)
