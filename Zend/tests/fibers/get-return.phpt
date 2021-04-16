--TEST--
Test fiber return value
--FILE--
<?php

$fiber = new Fiber(function (): int {
    $value = Fiber::suspend(1);
    return $value;
});

$value = $fiber->start();
var_dump($value);
var_dump($fiber->resume($value + 1));
var_dump($fiber->getReturn());

--EXPECT--
int(1)
NULL
int(2)
