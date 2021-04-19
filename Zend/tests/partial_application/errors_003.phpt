--TEST--
Partial application errors: missing parameters
--FILE--
<?php
function foo($a, ...$b) {

}

function bar($a, $b, $c) {}

$foo = foo(?);

try {
    $foo();
} catch (Error $ex) {
    printf("%s\n", $ex->getMessage());
}

$foo = foo(?, ?);

try {
    $foo(1);    
} catch (Error $ex) {
    printf("%s\n", $ex->getMessage());
}

$bar = bar(?, ?, ...);

try {
    $bar(1);
} catch (Error $ex) {
    printf("%s\n", $ex->getMessage());
}

$usleep = usleep(...);

try {
    $usleep();
} catch (Error $ex) {
    printf("%s\n", $ex->getMessage());
}

class Foo {
    public function bar($a, ...$b) {}
}

$foo = new Foo;

$bar = $foo->bar(?);

try {
    $bar();
} catch (Error $ex) {
    printf("%s\n", $ex->getMessage());
}

?>
--EXPECTF--
not enough arguments for application of foo, 0 given and exactly 1 expected, declared in %s on line 8
not enough arguments for application of foo, 1 given and exactly 2 expected, declared in %s on line 16
not enough arguments for application of bar, 1 given and at least 2 expected, declared in %s on line 24
not enough arguments for implementation of usleep, 0 given and exactly 1 expected
not enough arguments for application of Foo::bar, 0 given and exactly 1 expected, declared in %s on line 46


