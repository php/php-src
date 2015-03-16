--TEST--
Exceeding arg count check should not affect variadic functions or methods
--FILE--
<?php

function fn(...$arg) {
    echo __FUNCTION__, PHP_EOL;
}

class Obj {
    public function variadic(...$arg){
        echo __METHOD__, PHP_EOL;       
    }
}

fn();
fn(1);
fn(1, 2);

(new Obj)->variadic();
(new Obj)->variadic(1);
(new Obj)->variadic(1, 2);

echo PHP_EOL, "Done", PHP_EOL;

--EXPECTF--
fn
fn
fn
Obj::variadic
Obj::variadic
Obj::variadic

Done
