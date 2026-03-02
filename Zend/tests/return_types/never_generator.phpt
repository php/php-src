--TEST--
never return type: not valid for a generator
--FILE--
<?php

function generateList(string $uri): never {
    yield 1;
    exit();
}
?>
--EXPECTF--
Fatal error: Generator return type must be a supertype of Generator, never given in %s on line %d
