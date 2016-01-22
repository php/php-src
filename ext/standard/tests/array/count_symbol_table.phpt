--TEST--
Test count() function : count on symbol table
--FILE--
<?php

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
