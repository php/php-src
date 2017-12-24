--TEST--
invalid scalar return type constraint values (strict mode)
--FILE--
<?php

declare(strict_types = 1);

function f($p): scalar {
    return $p;
}

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
Return value of f() must be of the type scalar, array returned
Return value of f() must be of the type scalar, null returned
Return value of f() must be of the type scalar, object returned
Return value of f() must be of the type scalar, object returned
Return value of f() must be of the type scalar, resource returned
Return value of f() must be of the type scalar, resource returned
