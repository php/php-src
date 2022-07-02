--TEST--
Error suppression for types on variadic arguments works
--FILE--
<?php

function test(array... $args) {
    var_dump($args);
}

try {
    test([0], [1], 2);
} catch(Error $e) {
    var_dump($e->getMessage());
}

?>
--EXPECTF--
string(%d) "test(): Argument #3 must be of type array, int given, called in %s on line %d"
