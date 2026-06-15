--TEST--
Typed local variables: declaration without initializer, then assignment
--FILE--
<?php
int $x;
$x = 9;
var_dump($x);
?>
--EXPECT--
int(9)
