--TEST--
ReflectionFiber basic tests
--FILE--
<?php

$fiber = new Fiber(function (): void {
    $fiber = Fiber::this();
    echo "\nWithin Fiber:\n";
    var_dump($fiber->isStarted());
    var_dump($fiber->isRunning());
    var_dump($fiber->isSuspended());
    var_dump($fiber->isTerminated());
    Fiber::suspend();
});

$reflection = new ReflectionFiber($fiber);

echo "Before Start:\n";
var_dump($fiber === $reflection->getFiber());
var_dump($reflection->isStarted());
var_dump($reflection->isRunning());
var_dump($reflection->isSuspended());
var_dump($reflection->isTerminated());

$fiber->start();

echo "\nAfter Start:\n";
var_dump($reflection->isStarted());
var_dump($reflection->isRunning());
var_dump($reflection->isSuspended());
var_dump($reflection->isTerminated());
var_dump($reflection->getExecutingFile());
var_dump($reflection->getExecutingLine());
var_dump($reflection->getTrace());

$fiber->resume();

echo "\nAfter Resume:\n";
var_dump($fiber->isStarted());
var_dump($fiber->isRunning());
var_dump($fiber->isSuspended());
var_dump($fiber->isTerminated());

?>
--EXPECTF--
Before Start:
bool(true)
bool(false)
bool(false)
bool(false)
bool(false)

Within Fiber:
bool(true)
bool(true)
bool(false)
bool(false)

After Start:
bool(true)
bool(false)
bool(true)
bool(false)
string(%d) "%sReflectionFiber_basic.php"
int(10)
array(2) {
  [0]=>
  array(6) {
    ["file"]=>
    string(%d) "%sReflectionFiber_basic.php"
    ["line"]=>
    int(10)
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

After Resume:
bool(true)
bool(false)
bool(false)
bool(true)
