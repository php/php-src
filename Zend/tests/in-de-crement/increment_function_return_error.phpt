--TEST--
It's not possible to increment the return value of a function
--FILE--
<?php

function test() {
    return 42;
}

++test();

?>
--EXPECTF--
Fatal error: Can't use function return value in write context in %s on line %d
