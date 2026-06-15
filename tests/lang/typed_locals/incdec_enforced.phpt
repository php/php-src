--TEST--
Typed local variables: ++ and -- enforce the declared type (pre/post)
--FILE--
<?php
// pre/post increment and decrement keep an int an int
int $x = 1;
var_dump(++$x);     // int(2)
var_dump($x++);     // int(2), $x now 3
var_dump($x);       // int(3)
var_dump(--$x);     // int(2)
var_dump($x--);     // int(2), $x now 1
var_dump($x);       // int(1)

// float increments stay float
float $f = 1.5;
++$f;
var_dump($f);       // float(2.5)

// weak-mode: a float local incremented stays float
float $g = 2.0;
$g++;
var_dump($g);       // float(3)
?>
--EXPECT--
int(2)
int(2)
int(3)
int(2)
int(2)
int(1)
float(2.5)
float(3)
