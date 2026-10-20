--TEST--
Disallows using variables.
--FILE--
<?php

$foo = "bar";

const Closure = static function () use ($foo) {
    echo $foo, PHP_EOL;
};

var_dump(Closure);
(Closure)();

?>
--EXPECTF--
Fatal error: Cannot use(...) variables in constant expression in %s on line %d
