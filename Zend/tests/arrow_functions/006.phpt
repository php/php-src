--TEST--
Nested arrow function closing over variable
--FILE--
<?php

$b = 1;

var_dump((fn() => fn() => $b)()());
var_dump((fn() => function() use($b) { return $b; })()());

?>
--EXPECT--
int(1)
int(1)
