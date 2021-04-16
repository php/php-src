--TEST--
ReflectionFiber basic tests
--FILE--
<?php

$fiber = new Fiber(function (): void {
    $fiber = Fiber::this();
    var_dump($fiber->isStarted());
    var_dump($fiber->isRunning());
    var_dump($fiber->isSuspended());
    var_dump($fiber->isTerminated());
    Fiber::suspend();
});

$reflection = new ReflectionFiber($fiber);

var_dump($fiber === $reflection->getFiber());

var_dump($reflection->isStarted());
var_dump($reflection->isRunning());
var_dump($reflection->isSuspended());
var_dump($reflection->isTerminated());

$fiber->start();

var_dump($reflection->isStarted());
var_dump($reflection->isRunning());
var_dump($reflection->isSuspended());
var_dump($reflection->isTerminated());

var_dump($reflection->getExecutingFile());
var_dump($reflection->getExecutingLine());
var_dump($reflection->getTrace());

$fiber->resume();

var_dump($fiber->isStarted());
var_dump($fiber->isRunning());
var_dump($fiber->isSuspended());
var_dump($fiber->isTerminated());

--EXPECTF--
bool(true)
bool(false)
bool(false)
bool(false)
bool(false)
bool(true)
bool(true)
bool(false)
bool(false)
bool(true)
bool(false)
bool(true)
bool(false)
string(%d) "%sReflectionFiber_basic.php"
int(9)
array(2) {
  [0]=>
  array(6) {
    ["file"]=>
    string(%d) "%sReflectionFiber_basic.php"
    ["line"]=>
    int(9)
    ["function"]=>
    string(7) "suspend"
    ["class"]=>
    string(5) "Fiber"
    ["type"]=>
    string(2) "::"
    ["args"]=>
    array(0) {
    }
  }
  [1]=>
  array(2) {
    ["function"]=>
    string(9) "{closure}"
    ["args"]=>
    array(0) {
    }
  }
}
bool(true)
bool(false)
bool(false)
bool(true)