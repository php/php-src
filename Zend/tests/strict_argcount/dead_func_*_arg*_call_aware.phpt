--TEST--
Exceeding arg count check should be func_get_args() call aware even when it's on dead code
--FILE--
<?php

function fn($arg) {
    if(false){
        func_get_args();
    }
    echo __FUNCTION__, PHP_EOL;
}

fn(1);
fn();
fn(1, 2);

function fn2($arg) {
    if(false){
        func_get_arg(0);
    }
    echo __FUNCTION__, PHP_EOL;
}

fn2(1);
fn2(1, 2);

echo PHP_EOL, "Done", PHP_EOL;

--EXPECTF--
fn

Warning: Missing argument 1 for fn(), called in %s on line %d and defined in %s on line 3
fn
fn
fn2
fn2

Done
