--TEST--
Test warning on function calls with exceeding arg count
--FILE--
<?php

function fn() {
    echo __FUNCTION__, PHP_EOL;
}

fn();
fn(1);
call_user_func_array("fn", [1]);

function fn1($arg) {
    echo __FUNCTION__, PHP_EOL;
}

fn1(1);
fn1();
fn1(1, 2);
call_user_func_array("fn1", [1, 2]);


// test plural error message

function fn2($arg, $arg2) {
    echo __FUNCTION__, PHP_EOL;
}

fn2(1, 2);
fn2(1);
fn2(1, 2, 3);
call_user_func_array("fn2", [1, 2, 3]);

echo PHP_EOL, "Done", PHP_EOL;

--EXPECTF--
fn

Warning: fn() expects 0 parameters, 1 given, defined in %s on line 3 and called in %s on line 8
fn

Warning: fn() expects 0 parameters, 1 given, defined in %s on line 3 and called in %s on line 9
fn
fn1

Warning: Missing argument 1 for fn1(), called in %s on line 16 and defined in %s on line 11
fn1

Warning: fn1() expects at most 1 parameter, 2 given, defined in %s on line 11 and called in %s on line 17
fn1

Warning: fn1() expects at most 1 parameter, 2 given, defined in %s on line 11 and called in %s on line 18
fn1
fn2

Warning: Missing argument 2 for fn2(), called in %s on line 28 and defined in %s on line 23
fn2

Warning: fn2() expects at most 2 parameters, 3 given, defined in %s on line 23 and called in %s on line 29
fn2

Warning: fn2() expects at most 2 parameters, 3 given, defined in %s on line 23 and called in %s on line 30
fn2

Done
