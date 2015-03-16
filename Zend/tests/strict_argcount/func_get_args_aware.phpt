--TEST--
Test if exceeding arg count check is func_get_args() aware
--FILE--
<?php

function fn($arg) {
    $args = func_get_args();
    echo __FUNCTION__, PHP_EOL;
}

fn(1);
fn();
fn(1, 2);

echo PHP_EOL, "Done", PHP_EOL;

--EXPECTF--
fn

Warning: Missing argument 1 for fn(), called in %s on line 9 and defined in %s on line 3
fn
fn

Done
