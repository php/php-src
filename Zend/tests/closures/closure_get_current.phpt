--TEST--
Closure::getCurrent()
--FILE--
<?php

$i = 1;

$c = function ($p) use (&$i) {
    $self = Closure::getCurrent();
    var_dump($p, $i);
    $i++;
    if ($p < 10) {
        $self($p + 1);
    }
};

$c(1);
var_dump($i);

function fail() {
    Closure::getCurrent();
}

try {
    fail();
} catch (Error $e) {
    echo $e->getMessage(), "\n";
}

function foo() {
    var_dump(Closure::getCurrent());
}

try {
    foo(...)();
} catch (Error $e) {
    echo $e->getMessage(), "\n";
}

?>
--EXPECT--
int(1)
int(1)
int(2)
int(2)
int(3)
int(3)
int(4)
int(4)
int(5)
int(5)
int(6)
int(6)
int(7)
int(7)
int(8)
int(8)
int(9)
int(9)
int(10)
int(10)
int(11)
Current function is not a closure
Current function is not a closure
