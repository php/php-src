--TEST--
Basic arrow functionality check
--FILE--
<?php

error_reporting(E_ALL);

$foo = fn () => 1;
var_dump($foo());

--EXPECT--
int(1)
