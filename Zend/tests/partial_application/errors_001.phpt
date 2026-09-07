--TEST--
PFA errors: PFA instantiation follows the usual argument count validation
--FILE--
<?php
function foo($a, $b, $c) {

}

class C {
    function f($a, $b, $c) {
    }
}

try {
    foo(?);
} catch (Error $ex) {
    printf("%s: %s\n", $ex::class, $ex->getMessage());
}

try {
    foo(?, ?, ?, ?);
} catch (Error $ex) {
    printf("%s: %s\n", $ex::class, $ex->getMessage());
}

try {
    $c = new C();
    $c->f(?);
} catch (Error $ex) {
    printf("%s: %s\n", $ex::class, $ex->getMessage());
}

try {
    property_exists(?);
} catch (Error $ex) {
    printf("%s: %s\n", $ex::class, $ex->getMessage());
}

try {
    usleep(?, ?);
} catch (Error $ex) {
    printf("%s: %s\n", $ex::class, $ex->getMessage());
}

try {
    foo(?, ?, ?, ?, ...);
} catch (Error $ex) {
    printf("%s: %s\n", $ex::class, $ex->getMessage());
}

/* It is allowed to specify less than the number of required params, when there
 * is a variadic placeholder */
foo(?, ...);

?>
--EXPECT--
ArgumentCountError: Partial application of foo() expects exactly 3 arguments, 1 given
ArgumentCountError: Partial application of foo() expects at most 3 arguments, 4 given
ArgumentCountError: Partial application of C::f() expects exactly 3 arguments, 1 given
ArgumentCountError: Partial application of property_exists() expects exactly 2 arguments, 1 given
ArgumentCountError: Partial application of usleep() expects at most 1 arguments, 2 given
ArgumentCountError: Partial application of foo() expects at most 3 arguments, 4 given
