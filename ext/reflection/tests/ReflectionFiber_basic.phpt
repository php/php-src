--TEST--
ReflectionFiber basic tests
--FILE--
<?php

$callable = function (): void {
    $reflection = new ReflectionFiber(Fiber::getCurrent());
    echo "\nWithin Fiber:\n";
    var_dump($reflection->getExecutingFile());
    var_dump($reflection->getExecutingLine());
    var_dump($reflection->getTrace());
    Fiber::suspend();
};

$fiber = new Fiber($callable);

$reflection = new ReflectionFiber($fiber);

echo "Before Start:\n";
var_dump($fiber === $reflection->getFiber());
var_dump($callable === $reflection->getCallable());

$fiber->start();

echo "\nAfter Start:\n";
var_dump($reflection->getExecutingFile());
var_dump($reflection->getExecutingLine());
var_dump($callable === $reflection->getCallable());
var_dump($reflection->getTrace());

$fiber->resume();

echo "\nAfter Resume:\n";
$reflection->getTrace();

?>
--EXPECTF--
Before Start:
bool(true)
bool(true)

Within Fiber:
string(%d) "%sReflectionFiber_basic.php"
int(7)
array(2) {
  [0]=>
  array(7) {
    ["file"]=>
    string(%d) "%sReflectionFiber_basic.php"
    ["line"]=>
    int(8)
    ["function"]=>
    string(8) "getTrace"
    ["class"]=>
    string(15) "ReflectionFiber"
    ["object"]=>
    object(ReflectionFiber)#4 (0) {
    }
    ["type"]=>
    string(2) "->"
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

After Start:
string(%d) "%sReflectionFiber_basic.php"
int(9)
bool(true)
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

After Resume:

Fatal error: Uncaught Error: Cannot fetch information from a fiber that has not been started or is terminated in %sReflectionFiber_basic.php:%d
Stack trace:
#0 %sReflectionFiber_basic.php(%d): ReflectionFiber->getTrace()
#1 {main}
  thrown in %sReflectionFiber_basic.php on line %d
