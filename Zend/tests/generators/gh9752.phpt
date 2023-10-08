--TEST--
Bug GH-9752 (Generator crashes when interrupted during a function call with extra named params)
--FILE--
<?php

function f(...$x) {
}

function g() {
    f(a: 1, b: yield);
};

$gen = g();

foreach ($gen as $value) {
    break;
}

?>
==DONE==
--EXPECT--
==DONE==
