--TEST--
Arity-1 arrow functionality check
--FILE--
<?php

error_reporting(E_ALL);

$foo = fn ($x) => $x;
var_dump($foo(1));

--EXPECT--
int(1)
