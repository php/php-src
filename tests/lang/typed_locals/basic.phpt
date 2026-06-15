--TEST--
Typed local variables: basic declaration and var_dump of all four scalar types
--FILE--
<?php
string $s = 'hi';
int $i = 3;
float $f = 1.5;
bool $b = true;
var_dump($s, $i, $f, $b);
?>
--EXPECT--
string(2) "hi"
int(3)
float(1.5)
bool(true)
