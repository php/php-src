--TEST--
Test if exceeding arg count check is func_get_arg() aware
--FILE--
<?php

function fn($arg) {
    $arg = func_get_arg(0);
    echo __FUNCTION__, PHP_EOL;
}

fn(1);
fn(1, 2);

echo PHP_EOL, "Done", PHP_EOL;

--EXPECTF--
fn
fn

Done
