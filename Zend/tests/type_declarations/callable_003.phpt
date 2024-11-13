--TEST--
callable type#003
--FILE--
<?php

function foo(callable $a, $b, callable $c) {
    var_dump($a, $b, $c);
}
function bar(callable $a = null) {
    var_dump($a);
}

foo("strpos", 123, "strpos");
bar("substr");
?>
--EXPECTF--
Deprecated: bar(): Implicitly marking parameter $a as nullable is deprecated, the explicit nullable type must be used instead in %s on line %d
string(6) "strpos"
int(123)
string(6) "strpos"
string(6) "substr"
