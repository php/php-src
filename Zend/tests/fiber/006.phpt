--TEST--
tests Fiber status
--FILE--
<?php
$f = new Fiber(function () {
    await;
});

var_dump($f->status() == Fiber::STATUS_SUSPENDED);
$f->resume();
var_dump($f->status() == Fiber::STATUS_SUSPENDED);
$f->resume();
var_dump($f->status() == Fiber::STATUS_FINISHED);
$f->resume();

$f = new Fiber(function () {
    throw new Exception;
});
try {
    $f->resume();
} catch (Exception $e) {
}
var_dump($f->status() == Fiber::STATUS_DEAD);
--EXPECTF--
bool(true)
bool(true)
bool(true)
bool(true)
