--TEST--
Memory leak when returning TMP/VAR with wrong return type
--FILE--
<?php

function foo(): stdClass {
    $a = new stdClass;
    $b = [];
    return [$a, $b];
}

try {
    foo();
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), ' in ', $e->getFile(), ' on line ', $e->getLine(), "\n";
}

?>
--EXPECTF--
TypeError: foo(): Return value must be of type stdClass, array returned in %s on line %d
