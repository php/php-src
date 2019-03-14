--TEST--
Nested arrow function closing over variable
--FILE--
<?php

$b = 1;

var_dump((() ==> () ==> $b)()());
var_dump((() ==> function() use($b) { return $b; })()());

?>
--EXPECT--
int(1)
int(1)
