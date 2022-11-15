--TEST--
iterable type with object and class T should be redundant
--FILE--
<?php

function bar(): T|object|iterable|null {
    return null;
}

?>
--EXPECTF--
Fatal error: Type T|Traversable|object|array|null contains both object and a class type, which is redundant in %s on line %d
