--TEST--
Scalar type hint aliases
--FILE--
<?php

function foo(integer $a, boolean $b) {
    var_dump($a, $b);
}

function bar(int $a, bool $b) {
    var_dump($a, $b);
}

foo(1, true);
bar(1, true);
--EXPECT--
int(1)
bool(true)
int(1)
bool(true)
