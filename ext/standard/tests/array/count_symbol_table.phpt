--TEST--
Test count() function : count on symbol table
--FILE--
<?php
/**
 * Prototype  : int count(array|Countable|null $var [, int $mode])
 * Description: Count the number of elements in a variable (usually an array)
 * Source code: ext/standard/array.c
 */

$c1 = 0;
$c2 = 0;
$a = 1;
$b = 1;
$c1 = count($GLOBALS);
unset($a);
unset($GLOBALS["b"]);
$c2 = count($GLOBALS);

var_dump($c1 - $c2);
$c = 1;
$c1 = count($GLOBALS);
var_dump($c1 - $c2);
?>
--EXPECT--
int(2)
int(1)
