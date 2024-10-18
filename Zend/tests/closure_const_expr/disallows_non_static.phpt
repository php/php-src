--TEST--
Disallows using variables.
--FILE--
<?php

$foo = "bar";

const Closure = function () {
    echo $foo, PHP_EOL;
};

var_dump(Closure);
(Closure)();

?>
--EXPECTF--
Fatal error: Closures in constant expressions must be "static" in %s on line %d
