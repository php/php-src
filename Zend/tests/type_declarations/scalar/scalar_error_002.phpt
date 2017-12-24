--TEST--
invalid scalar parameter type constraint values (strict mode)
--FILE--
<?php

declare(strict_types = 1);

function f(scalar $p) {}

function provide_non_scalar_types() {
    yield [];
    yield null;
    yield new stdClass;
    yield new class { function __toString() { return 'obj'; }};
    $h = fopen('php://stdout', 'w');
    yield $h;
    fclose($h);
    yield $h;
}

foreach (provide_non_scalar_types() as $t) {
    try {
        f($t);
    }
    catch (TypeError $e) {
        echo "{$e->getMessage()}\n";
    }
}

?>
--EXPECTF--
Argument 1 passed to f() must be of the type scalar, array given, called in %s
Argument 1 passed to f() must be of the type scalar, null given, called in %s
Argument 1 passed to f() must be of the type scalar, object given, called in %s
Argument 1 passed to f() must be of the type scalar, object given, called in %s
Argument 1 passed to f() must be of the type scalar, resource given, called in %s
Argument 1 passed to f() must be of the type scalar, resource given, called in %s
