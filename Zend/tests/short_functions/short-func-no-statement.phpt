--TEST--
Non-expression statements parse error in short functions.
--FILE--
<?php

function test(array $a) => foreach ($a as $v) print $v;

test([1, 2, 3]);

?>
--EXPECTF--
Parse error: syntax error, unexpected token "foreach" in %s on line %d
