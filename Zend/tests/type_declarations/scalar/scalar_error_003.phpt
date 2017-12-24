--TEST--
invalid scalar return type constraint values (weak mode)
--FILE--
<?php

function f($p): scalar {
    return $p;
}

function provide_non_scalar_types() {
    yield [];
    yield null;
    yield new stdClass;
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
Return value of f() must be of the type scalar, resource returned
Return value of f() must be of the type scalar, resource returned
