--TEST--
PFA errors: PFA call follows the usual argument count validation
--FILE--
<?php
function foo($a, ...$b) {

}

function bar($a, $b, $c) {}

$foo = foo(?);

try {
    $foo();
} catch (Error $ex) {
    printf("%s: %s\n", $ex::class, $ex->getMessage());
}

$foo = foo(?, ?);

try {
    $foo(1);
} catch (Error $ex) {
    printf("%s: %s\n", $ex::class, $ex->getMessage());
}

$bar = bar(?, ?, ...);

try {
    $bar(1);
} catch (Error $ex) {
    printf("%s: %s\n", $ex::class, $ex->getMessage());
}

class Foo {
    public function bar($a, ...$b) {}
}

$foo = new Foo;

$bar = $foo->bar(?);

try {
    $bar();
} catch (Error $ex) {
    printf("%s: %s\n", $ex::class, $ex->getMessage());
}

$repeat = str_repeat('a', ...);

try {
    $repeat();
} catch (Error $ex) {
    printf("%s: %s\n", $ex::class, $ex->getMessage());
}

$usleep = usleep(?);

try {
    $usleep();
} catch (Error $ex) {
    printf("%s: %s\n", $ex::class, $ex->getMessage());
}

try {
    $usleep(1, 2);
} catch (Error $ex) {
    printf("%s: %s\n", $ex::class, $ex->getMessage());
}
?>
--EXPECTF--
ArgumentCountError: Too few arguments to function {closure:%s:%d}(), 0 passed in %s on line %d and exactly 1 expected
ArgumentCountError: Too few arguments to function {closure:%s:%d}(), 1 passed in %s on line %d and exactly 2 expected
ArgumentCountError: Too few arguments to function {closure:%s:%d}(), 1 passed in %s on line %d and exactly 3 expected
ArgumentCountError: Too few arguments to function Foo::{closure:%s:%d}(), 0 passed in %s on line %d and exactly 1 expected
ArgumentCountError: Too few arguments to function {closure:%s:%d}(), 0 passed in %s on line %d and exactly 1 expected
ArgumentCountError: Too few arguments to function {closure:%s:%d}(), 0 passed in %s on line %d and exactly 1 expected
