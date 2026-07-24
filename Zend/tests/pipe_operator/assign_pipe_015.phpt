--TEST--
Pipe assign operator error: cannot use function call as target
--FILE--
<?php

function getVal(): string { return "hello"; }

getVal() |>= strtoupper(...);

?>
--EXPECTF--
Fatal error: Can't use function return value in write context in %s on line %d
