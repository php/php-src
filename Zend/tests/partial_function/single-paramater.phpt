--TEST--
Single parameter function.
--FILE--
<?php

function test1(int $a) {
    return $a + 1;
}

$p = test1(?);

print p(3);

--EXPECTF--
4
