--TEST--
scalar static method return type constraint
--DESCRIPTION--
Tests that a static method that has a scalar return type constraint permits
only compatible types to be returned.
--FILE--
<?php

class C {
    static function f($p): scalar {
        return $p;
    }
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
        C::f($t);
    }
    catch (TypeError $e) {
        echo "{$e->getMessage()}\n";
    }
}

?>
--EXPECTF--
Return value of C::f() must be of the type scalar, array returned
Return value of C::f() must be of the type scalar, null returned
Return value of C::f() must be of the type scalar, object returned
Return value of C::f() must be of the type scalar, resource returned
Return value of C::f() must be of the type scalar, resource returned
