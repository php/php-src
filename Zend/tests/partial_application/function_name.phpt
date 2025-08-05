--TEST--
Partial application function name
--FILE--
<?php

function g($a) {}

function f() {
    var_dump((new ReflectionFunction(g(?)))->getName());
}

f();

var_dump((new ReflectionFunction(g(?)))->getName());

?>
--EXPECTF--
string(%d) "{closure:pfa:f():6}"
string(%d) "{closure:pfa:%sfunction_name.php:11}"
