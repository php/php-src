--TEST--
Typed local variables: weak-mode coercion on initialization
--FILE--
<?php
string $s = 5;
int $i = "42";
float $f = 3;
var_dump($s, $i, $f);
?>
--EXPECT--
string(1) "5"
int(42)
float(3)
